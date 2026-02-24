package pl.miloszgilga.htas.client.update

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.parse
import pl.miloszgilga.htas.client.util.UiText
import java.util.concurrent.TimeUnit

class FirmwareUpdateManager(
  private val jsonParser: JsonParser,
  private val repoOwner: String = "milosz08",
  private val repoName: String = "home-theater-automation-system"
) {
  companion object {
    private const val TAG = "UpdateManager"
    private const val CHECK_INTERVAL_MS = 5L * 60L * 1000L // 5 minutes
  }

  private val client = OkHttpClient.Builder()
    .connectTimeout(10, TimeUnit.SECONDS)
    .build()

  private val _latestVersion = MutableStateFlow<String?>(null)

  val latestVersion = _latestVersion.asStateFlow()

  private var deviceVersion: String? = null
  private var latestGithubVersion: String? = null
  private var checkJob: Job? = null
  private var isPaused = false

  fun startPeriodicChecks(scope: CoroutineScope) {
    checkJob?.cancel()
    checkJob = scope.launch(Dispatchers.IO) {
      while (isActive) {
        if (!isPaused) {
          fetchLatestRelease()
          checkIfUpdateNeeded()
        }
        delay(CHECK_INTERVAL_MS)
      }
    }
  }

  fun pauseChecks() {
    isPaused = true
    _latestVersion.value = null
    Log.d(TAG, "updates checker paused")
  }

  fun resumeChecks() {
    isPaused = false
    Log.d(TAG, "updates checker resumed")
  }

  fun hideBanner() {
    _latestVersion.value = null
    Log.d(TAG, "update banner hided manually")
  }

  fun manualCheck(scope: CoroutineScope) {
    isPaused = false
    scope.launch(Dispatchers.IO) {
      fetchLatestRelease()
      checkIfUpdateNeeded()
    }
  }

  fun onDeviceVersionReceived(version: String) {
    deviceVersion = version
    checkIfUpdateNeeded()
  }

  suspend fun downloadAndVerifyFirmware(
    onProgress: suspend (UiText) -> Unit,
    onError: suspend (UiText) -> Unit,
  ): ByteArray? {
    return withContext(Dispatchers.IO) {
      try {
        onProgress(UiText.StringResource(R.string.ota_step_fetching_info))
        val release = fetchReleaseInfo()

        val binAsset = release.assets.find { it.name.endsWith(".bin") }
        if (binAsset == null) {
          onError(UiText.StringResource(R.string.error_ota_bin_not_found))
          return@withContext null
        }
        val hashAsset = release.assets.find { it.name.endsWith(".txt") }
        if (hashAsset == null) {
          onError(UiText.StringResource(R.string.error_ota_hash_not_found))
          return@withContext null
        }
        onProgress(UiText.StringResource(R.string.ota_step_downloading_hash))
        val hashFileContent = fetchBytes(hashAsset.downloadUrl)?.decodeToString()
        if (hashFileContent == null) {
          onError(UiText.StringResource(R.string.error_ota_hash_download_failed))
          return@withContext null
        }
        val hashParts = hashFileContent.split(":")
        if (hashParts.size != 2) {
          onError(UiText.StringResource(R.string.error_ota_invalid_hash_format))
          return@withContext null
        }
        val algorithmFromFile = hashParts[0].trim()
        val expectedHash = hashParts[1].trim()

        onProgress(UiText.StringResource(R.string.ota_step_downloading_firmware))
        val binBytes = fetchBytes(binAsset.downloadUrl)
        if (binBytes == null) {
          onError(UiText.StringResource(R.string.error_ota_firmware_download_failed))
          return@withContext null
        }
        onProgress(UiText.StringResource(R.string.ota_step_verifying))
        val actualHash = calculateHash(binBytes, algorithmFromFile)

        if (actualHash.equals(expectedHash, ignoreCase = true)) {
          onProgress(UiText.StringResource(R.string.ota_step_sending_to_device))
          return@withContext binBytes
        } else {
          onError(UiText.StringResource(R.string.error_ota_hash_mismatch))
          return@withContext null
        }
      } catch (e: Exception) {
        onError(UiText.StringResource(R.string.error_network_generic, e.localizedMessage ?: "?"))
        return@withContext null
      }
    }
  }

  private fun fetchLatestRelease() {
    try {
      val release = fetchReleaseInfo()
      latestGithubVersion = release.tagName
      Log.d(TAG, "newest version on github: $latestGithubVersion")
    } catch (ex: Exception) {
      Log.e(TAG, "unable to fetch newest version from github: ${ex.message}")
    }
  }

  private fun fetchReleaseInfo(): GithubRelease {
    val request = Request.Builder()
      .url("https://api.github.com/repos/$repoOwner/$repoName/releases/latest")
      .header("Accept", "application/vnd.github.v3+json")
      .build()
    client.newCall(request).execute().use { response ->
      if (!response.isSuccessful) {
        throw Exception("GitHub API error: ${response.code}")
      }
      val body = response.body.string()
      return jsonParser.parse<GithubRelease>(body)
    }
  }

  private fun fetchBytes(url: String): ByteArray? {
    val request = Request.Builder().url(url).build()
    client.newCall(request).execute().use { response ->
      if (!response.isSuccessful) {
        return null
      }
      return response.body.bytes()
    }
  }

  private fun calculateHash(data: ByteArray, algorithm: String): String {
    val digest = java.security.MessageDigest.getInstance(algorithm)
    val hashBytes = digest.digest(data)
    return hashBytes.joinToString("") { "%02x".format(it) }
  }

  private fun cleanVersionString(version: String): String {
    return version.substringBefore('-').substringBefore('+').replace(Regex("[^0-9.]"), "")
  }

  private fun checkIfUpdateNeeded() {
    val latest = latestGithubVersion ?: return
    val current = deviceVersion ?: return
    _latestVersion.value = getHigherVersionOrNull(latest, current)
  }

  private fun getHigherVersionOrNull(latest: String, current: String): String? {
    val cleanLatest = cleanVersionString(latest)
    val cleanCurrent = cleanVersionString(current)

    val latestParts = cleanLatest.split(".").mapNotNull { it.toIntOrNull() }
    val currentParts = cleanCurrent.split(".").mapNotNull { it.toIntOrNull() }

    val length = maxOf(latestParts.size, currentParts.size)
    for (i in 0 until length) {
      val l = latestParts.getOrElse(i) { 0 }
      val c = currentParts.getOrElse(i) { 0 }
      if (l > c) {
        return cleanLatest
      }
      if (l < c) {
        return null
      }
    }
    return null
  }
}
