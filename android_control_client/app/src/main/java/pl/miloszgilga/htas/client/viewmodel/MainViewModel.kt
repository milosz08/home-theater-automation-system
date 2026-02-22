package pl.miloszgilga.htas.client.viewmodel

import android.app.Application
import android.util.Log
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.application
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.initializer
import androidx.lifecycle.viewmodel.viewModelFactory
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import pl.miloszgilga.htas.client.net.GsonJsonParser
import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.NetworkEvent
import pl.miloszgilga.htas.client.net.ws.WsClient
import pl.miloszgilga.htas.client.net.ws.WsAction
import pl.miloszgilga.htas.client.net.ws.WsEvent
import pl.miloszgilga.htas.client.net.ws.WsEventParser
import pl.miloszgilga.htas.client.net.parse
import pl.miloszgilga.htas.client.store.SettingsStore
import pl.miloszgilga.htas.client.store.ServerConfig
import pl.miloszgilga.htas.client.util.UiText
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.net.TofuCertificateException
import pl.miloszgilga.htas.client.net.rest.HttpMethod
import pl.miloszgilga.htas.client.net.rest.ResetPasswordResponse
import pl.miloszgilga.htas.client.net.rest.RestExecutor
import pl.miloszgilga.htas.client.toast.ToastManager
import pl.miloszgilga.htas.client.toast.ToastType
import pl.miloszgilga.htas.client.util.mapDeviceError

class MainViewModel(
  application: Application,
  private val jsonParser: JsonParser,
) : AndroidViewModel(application) {
  private val store = SettingsStore(application)
  private val repository = WsClient(jsonParser)
  private val eventParser = WsEventParser(jsonParser)
  private val restExecutor = RestExecutor(jsonParser)

  private var connectionJob: Job? = null
  private var cooldownJob: Job? = null

  var uiState by mutableStateOf<AppUiState>(AppUiState.Loading)
    private set
  var sysInfo by mutableStateOf<WsEvent.SysInfo?>(null)
    private set
  val envHistory = mutableStateListOf<WsEvent.Env>()

  var lastConnectedTimestamp by mutableStateOf<Long?>(null)
    private set
  var isReconnecting by mutableStateOf(false)
    private set

  var isUiLocked by mutableStateOf(false)
    private set
  var cooldownProgress by mutableFloatStateOf(0f)
    private set
  var isCooldownEnabled by mutableStateOf(true)
    private set
  var cooldownDurationSec by mutableIntStateOf(SettingsStore.INIT_COOLDOWN_DURATION_SEC)
    private set

  companion object {
    private const val TAG = "MainViewModel"

    val Factory: ViewModelProvider.Factory = viewModelFactory {
      val jsonParser = GsonJsonParser()
      initializer {
        MainViewModel(
          (this[ViewModelProvider.AndroidViewModelFactory.APPLICATION_KEY] as Application),
          jsonParser
        )
      }
    }
    private const val MAX_HISTORY_SIZE = 50

    const val MIN_COOLDOWN_SEC = 1f
    const val MAX_COOLDOWN_SEC = 30f
    const val COOLDOWN_STEPS = 28
  }

  init {
    viewModelScope.launch {
      store.lastConnected.collect { lastConnectedTimestamp = it }
    }
    viewModelScope.launch {
      store.cooldownEnabled.collect { isCooldownEnabled = it }
    }
    viewModelScope.launch {
      store.cooldownDurationSec.collect { cooldownDurationSec = it }
    }
    viewModelScope.launch {
      val saved = store.savedConfig.first()
      if (saved != null) {
        Log.d(TAG, "found saved device config, attempting auto-connect")
        connectInternal(saved)
      } else {
        Log.d(TAG, "no saved device config found, setting state to unpaired")
        uiState = AppUiState.Unpaired
      }
    }
  }

  fun onQrScanned(json: String): Boolean {
    return try {
      val config = jsonParser.parse<ServerConfig>(json)
      viewModelScope.launch {
        Log.d(TAG, "saving new device config to datastore")
        store.saveConfig(config)
      }
      connectInternal(config)
      true
    } catch (ex: Exception) {
      Log.e(TAG, "failed to parse qr code json", ex)
      uiState = AppUiState.QrScanError(
        UiText.StringResource(R.string.error_qr_invalid_format)
      )
      false
    }
  }

  fun onQrScanFailure(errorMsg: String?) {
    val uiError = errorMsg?.let { UiText.DynamicString(it) }
      ?: UiText.StringResource(R.string.error_camera_general)
    Log.e(TAG, "qr scan failure reported from camera: $errorMsg")
    uiState = AppUiState.QrScanError(uiError)
  }

  fun retryConnection(config: ServerConfig) {
    Log.d(TAG, "user requested connection retry")
    connectInternal(config)
  }

  fun forgetDevice() {
    disconnect()
    viewModelScope.launch {
      store.clearConfig()
      Log.w(TAG, "forgetting current device and cleared datastore")
      uiState = AppUiState.Unpaired
    }
  }

  fun toggleCooldownEnabled(enabled: Boolean) {
    isCooldownEnabled = enabled
    viewModelScope.launch {
      store.saveCooldownEnabled(enabled)
    }
  }

  fun setCooldownDuration(seconds: Int) {
    cooldownDurationSec = seconds
    viewModelScope.launch {
      store.saveCooldownDuration(seconds)
    }
  }

  fun toggleUiLock() {
    cooldownJob?.cancel()
    cooldownProgress = 0f
    isUiLocked = !isUiLocked
    Log.d(TAG, "ui manually ${if (isUiLocked) "locked" else "unlocked"} by user")
  }

  fun sendCommand(action: WsAction, value: Any? = null) {
    if (isUiLocked) {
      Log.w(TAG, "ignored command ${action.key}, ui is in cooldown")
      return
    }
    if (isReconnecting) {
      Log.w(TAG, "ignored command ${action.key}, reconnecting")
      ToastManager.show(application.getString(R.string.reconnecting), ToastType.ERROR)
      return
    }
    val success = repository.sendAction(action, value)
    if (success && isCooldownEnabled) {
      startCooldown()
      return
    }
    Log.w(TAG, "unable to send command: ${action.key}")
    ToastManager.show(application.getString(R.string.send_failed), ToastType.ERROR)
  }

  fun disconnect() {
    connectionJob?.cancel()
    repository.disconnect()
    Log.d(TAG, "disconnected ws and cancelling jobs")
  }

  fun manuallyDisconnect(config: ServerConfig) {
    disconnect()
    Log.d(TAG, "user manually disconnected the device")
    uiState = AppUiState.Disconnected(config)
  }

  suspend fun resetPassword() {
    val currentConfig = getCurrentConfigBasedConnectionState() ?: return
    restExecutor.execute<ResetPasswordResponse>(
      config = currentConfig,
      endpoint = "/api/reset-password",
      method = HttpMethod.DELETE,
      onSuccess = { response ->
        if (response != null) {
          val newDefaultPassword = response.defaultPassword
          val updatedConfig = currentConfig.copy(pass = newDefaultPassword)
          store.saveConfig(updatedConfig)
          uiState = AppUiState.Connected(updatedConfig)
          Log.d(TAG, "successfully reset password")
          ToastManager.show(application.getString(R.string.password_reset))
        }
      },
      onError = { uiError ->
        ToastManager.show(uiError.asString(application), ToastType.ERROR)
      }
    )
  }

  suspend fun changeDevicePassword(oldPass: String, newPass: String) {
    val currentConfig = getCurrentConfigBasedConnectionState() ?: return
    restExecutor.execute<Unit>(
      config = currentConfig,
      endpoint = "/api/change-password",
      method = HttpMethod.POST,
      payload = mapOf("old" to oldPass, "new" to newPass),
      onSuccess = {
        val updatedConfig = currentConfig.copy(pass = newPass)
        store.saveConfig(updatedConfig)
        uiState = AppUiState.Connected(updatedConfig)
        Log.d(TAG, "successfully changed password")
        ToastManager.show(application.getString(R.string.password_changed))
      },
      onError = { uiError ->
        ToastManager.show(uiError.asString(application), ToastType.ERROR)
      }
    )
  }

  override fun onCleared() {
    super.onCleared()
    disconnect()
  }

  private fun startCooldown() {
    cooldownJob?.cancel()
    cooldownJob = viewModelScope.launch {
      isUiLocked = true
      val totalMs = cooldownDurationSec * 1000L
      val refreshRateMs = 30L
      var remainingMs = totalMs
      while (remainingMs > 0) {
        cooldownProgress = remainingMs.toFloat() / totalMs.toFloat()
        delay(refreshRateMs)
        remainingMs -= refreshRateMs
      }
      cooldownProgress = 0f
      isUiLocked = false
    }
  }

  private fun getCurrentConfigBasedConnectionState(): ServerConfig? {
    val currentState = uiState
    if (currentState !is AppUiState.Connected) {
      ToastManager.show(application.getString(R.string.device_is_not_connected), ToastType.ERROR)
      return null
    }
    return currentState.config
  }

  private fun connectInternal(config: ServerConfig) {
    connectionJob?.cancel()

    uiState = AppUiState.Connecting
    connectionJob = viewModelScope.launch {
      repository.connectWebSocket(config).collect { event ->
        when (event) {
          is NetworkEvent.Connected -> {
            isReconnecting = false
            Log.d(TAG, "NetworkEvent.Connected received, requesting system info")
            store.saveLastConnected(System.currentTimeMillis())
            repository.sendAction(WsAction.GET_SYS_INFO)
            uiState = AppUiState.Connected(config)
          }

          is NetworkEvent.Reconnecting -> {
            Log.w(TAG, "NetworkEvent.Reconnecting received, reconnect attempt")
            isReconnecting = true
          }

          is NetworkEvent.Error -> {
            Log.e(TAG, "NetworkEvent.Error received: ${event.throwable.message}")
            uiState = AppUiState.ConnectionFailed(config, mapErrorToUiText(event.throwable))
          }

          is NetworkEvent.Disconnected -> {
            Log.w(TAG, "NetworkEvent.Disconnected received, reason: ${event.reason}")
            val errorText = UiText.StringResource(R.string.error_network_disconnected)
            uiState = AppUiState.ConnectionFailed(config, errorText)
          }

          is NetworkEvent.MessageReceived -> {
            val wsEvent = eventParser.parseEvent(event.text)
            handleWsEvent(wsEvent)
          }
        }
      }
    }
  }

  private fun handleWsEvent(event: WsEvent) {
    when (event) {
      is WsEvent.SysInfo -> {
        sysInfo = event
      }

      is WsEvent.Env -> {
        Log.v(TAG, "handling env ws event, history size: ${envHistory.size}")
        envHistory.add(event)
        if (envHistory.size > MAX_HISTORY_SIZE) {
          envHistory.removeAt(0)
        }
      }

      is WsEvent.CommandInvocation -> {
        val action = HardwareCmd.fromKey(event.cmd)
        val isError = event.errorName != null && event.errorCode != null
        if (isError) {
          Log.e(TAG, "command: ${event.cmd} failed: ${event.errorName} (${event.errorCode})")
          ToastManager.show(mapDeviceError(event.errorName, event.errorCode).asString(application), ToastType.ERROR)
        } else {
          val cmdName = action?.let { application.getString(it.resId) } ?: event.cmd
          Log.d(TAG, "command: ${event.cmd} success")
          ToastManager.show(cmdName, ToastType.NORMAL)
        }
      }

      is WsEvent.Unknown -> {
        Log.w(TAG, "received unknown ws event type: ${event.type}")
      }

      is WsEvent.ParseError -> {
        Log.e(TAG, "error parsing ws event payload", event.cause)
      }
    }
  }

  private fun mapErrorToUiText(throwable: Throwable): UiText {
    return when (throwable) {
      is TofuCertificateException.NoCertificate -> {
        UiText.StringResource(R.string.error_cert_missing)
      }

      is TofuCertificateException.HashMismatch -> {
        UiText.StringResource(R.string.error_cert_mismatch, throwable.expected, throwable.actual)
      }

      else -> {
        UiText.StringResource(R.string.error_network_generic, throwable.localizedMessage ?: "?")
      }
    }
  }
}
