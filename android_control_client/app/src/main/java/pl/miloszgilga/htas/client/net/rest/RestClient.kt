package pl.miloszgilga.htas.client.net.rest

import android.util.Log
import kotlinx.coroutines.suspendCancellableCoroutine
import okhttp3.Call
import okhttp3.Callback
import okhttp3.FormBody
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.NetworkClient
import pl.miloszgilga.htas.client.store.ServerConfig
import java.io.IOException
import kotlin.coroutines.resume
import kotlin.coroutines.resumeWithException
import kotlin.jvm.java

class RestClient(
  @PublishedApi internal val httpClient: OkHttpClient,
  @PublishedApi internal val jsonParser: JsonParser,
) : NetworkClient() {
  companion object {
    @PublishedApi
    internal const val TAG = "RestClient"
  }

  suspend inline fun <reified T : Any> performRequest(
    config: ServerConfig,
    endpoint: String,
    method: HttpMethod = HttpMethod.GET,
    bodyPayload: Any? = null,
    contentType: String? = null,
  ): T? = suspendCancellableCoroutine { continuation ->
    val url = "https://${config.ip}:${config.port}$endpoint"

    val requestBody: RequestBody? = if (bodyPayload != null && method != HttpMethod.GET) {
      when (bodyPayload) {
        is Map<*, *> -> {
          val formBuilder = FormBody.Builder()
          bodyPayload.forEach { (key, value) ->
            formBuilder.add(key.toString(), value.toString())
          }
          formBuilder.build()
        }

        is ByteArray -> {
          val mediaType = (contentType ?: "application/octet-stream").toMediaTypeOrNull()
          bodyPayload.toRequestBody(mediaType)
        }

        else -> null
      }
    } else {
      null
    }

    val request = Request.Builder()
      .url(url)
      .addHeader(AUTH_HEADER_NAME, config.pass)
      .method(method.name, requestBody)
      .build()

    val call = httpClient.newCall(request)

    continuation.invokeOnCancellation {
      Log.d(TAG, "request to $url cancelled by coroutine scope")
      call.cancel()
    }

    call.enqueue(object : Callback {
      override fun onFailure(call: Call, e: IOException) {
        if (continuation.isActive) {
          continuation.resumeWithException(e)
        }
      }

      override fun onResponse(call: Call, response: Response) {
        response.use {
          if (!continuation.isActive) {
            Log.d(TAG, "response received for $url, but coroutine is already cancelled")
            return
          }
          Log.d(TAG, "response received for $url with http code: ${response.code}")
          val responseStr = it.body.string()
          if (!response.isSuccessful) {
            Log.e(TAG, "http error ${response.code}, attempting to parse payload")
            try {
              val espError = jsonParser.fromJson(responseStr, ErrorPayload::class.java)
              Log.e(TAG, "parsed rest error: ${espError.errorName} (${espError.errorCode})")
              continuation.resumeWithException(
                RestException(response.code, espError.errorCode, espError.errorName)
              )
            } catch (ex: Exception) {
              Log.e(TAG, "failed to parse error payload, raw body: $responseStr", ex)
              continuation.resumeWithException(
                RestException(response.code, "UNKNOWN", "Server returned ${response.code}")
              )
            }
            return
          }
          if (response.code == 204 || responseStr.isBlank()) {
            Log.d(TAG, "response is 204 No Content or empty, resuming with null")
            continuation.resume(null)
            return
          }
          try {
            val parsed = jsonParser.fromJson(responseStr, T::class.java)
            continuation.resume(parsed)
          } catch (ex: Exception) {
            Log.e(TAG, "failed to parse success response: ${ex.message}", ex)
            continuation.resumeWithException(ex)
          }
        }
      }
    })
  }
}
