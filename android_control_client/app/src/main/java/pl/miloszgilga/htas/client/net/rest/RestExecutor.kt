package pl.miloszgilga.htas.client.net.rest

import android.util.Log
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.TofuClient
import pl.miloszgilga.htas.client.store.ServerConfig
import pl.miloszgilga.htas.client.util.UiText
import pl.miloszgilga.htas.client.util.mapDeviceError
import java.net.SocketTimeoutException

class RestExecutor(@PublishedApi internal val jsonParser: JsonParser) {
  companion object {
    @PublishedApi
    internal const val TAG = "RestExecutor"
  }

  suspend inline fun <reified T : Any> execute(
    config: ServerConfig,
    endpoint: String,
    method: HttpMethod = HttpMethod.GET,
    payload: Map<String, String>? = null,
    crossinline onSuccess: suspend (response: T?) -> Unit,
    crossinline onError: suspend (errorText: UiText) -> Unit,
  ) {
    try {
      val secureClient = TofuClient(config.hash).getClient()
      val restClient = RestClient(secureClient, jsonParser)
      val result = restClient.performRequest<T>(
        config = config,
        endpoint = endpoint,
        method = method,
        bodyPayload = payload,
      )
      onSuccess(result)
    } catch (e: Exception) {
      Log.e(TAG, "request failed for endpoint: $endpoint", e)
      val uiTextError = mapRestErrorToUiText(e)
      onError(uiTextError)
    }
  }

  @PublishedApi
  internal fun mapRestErrorToUiText(throwable: Throwable): UiText {
    return when (throwable) {
      is SocketTimeoutException -> UiText.StringResource(R.string.error_esp_timeout)

      is RestException -> mapDeviceError(throwable.errorName, throwable.errorCode)

      else -> UiText.StringResource(
        R.string.error_network_generic,
        throwable.localizedMessage ?: "Unknown REST error"
      )
    }
  }
}
