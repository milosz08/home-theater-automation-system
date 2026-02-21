package pl.miloszgilga.htas.client.net.ws

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow
import kotlinx.coroutines.launch
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.NetworkClient
import pl.miloszgilga.htas.client.net.NetworkEvent
import pl.miloszgilga.htas.client.net.TofuClient
import pl.miloszgilga.htas.client.store.ServerConfig
import java.util.concurrent.TimeUnit

class WsClient(private val jsonParser: JsonParser) : NetworkClient() {
  companion object {
    private const val TAG = "NetworkRepository"

    private const val HEARTBEAT_TIMEOUT_MS = 12000L
    private const val REASON_SCOPE_CLOSED = "Scope closed"
    private const val REASON_USER_DISCONNECTED = "User disconnected"
  }

  private var webSocket: WebSocket? = null
  private var client: OkHttpClient? = null

  private var watchdogScope: CoroutineScope? = null
  private var watchdogJob: Job? = null

  fun connectWebSocket(config: ServerConfig): Flow<NetworkEvent> = callbackFlow {
    Log.d(TAG, "initiating ws connection to ${config.ip}:${config.port}")

    disconnect()
    watchdogScope = CoroutineScope(Dispatchers.Default + SupervisorJob())
    try {
      val tofuWrapper = TofuClient(config.hash)
      val baseClient = tofuWrapper.getClient()

      client = baseClient.newBuilder()
        .readTimeout(0, TimeUnit.MILLISECONDS)
        .connectTimeout(10, TimeUnit.SECONDS)
        .pingInterval(5, TimeUnit.SECONDS)
        .build()

      val request = Request.Builder()
        .url("wss://${config.ip}:${config.port}/ws/control")
        .addHeader(AUTH_HEADER_NAME, config.pass)
        .build()

      val listener = object : WebSocketListener() {
        override fun onOpen(webSocket: WebSocket, response: Response) {
          Log.d(TAG, "ws opened successfully")
          trySend(NetworkEvent.Connected)
          resetWatchdog()
        }

        override fun onMessage(webSocket: WebSocket, text: String) {
          Log.d(TAG, "ws received message: $text")
          resetWatchdog()
          trySend(NetworkEvent.MessageReceived(text))
        }

        override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
          Log.e(TAG, "ws failure: ${t.message}", t)
          trySend(NetworkEvent.Error(t))
          close()
        }

        override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
          Log.w(TAG, "ws closing by server, code: $code, reason: $reason")
          trySend(NetworkEvent.Disconnected(reason))
          close()
        }
      }
      webSocket = client?.newWebSocket(request, listener)
    } catch (ex: Exception) {
      Log.e(TAG, "exception during ws setup: ${ex.message}", ex)
      trySend(NetworkEvent.Error(ex))
      close()
    }
    awaitClose {
      Log.d(TAG, "flow scope closed, cleaning up ws")
      stopWatchdog()
      webSocket?.close(1000, REASON_SCOPE_CLOSED)
      webSocket = null
    }
  }

  fun disconnect() {
    if (webSocket != null) {
      Log.d(TAG, "manual disconnect triggered by user/system")
      webSocket?.close(1000, REASON_USER_DISCONNECTED)
      webSocket = null
      client = null
    }
  }

  fun sendAction(action: WsAction, value: Any? = null): Boolean {
    val socket = webSocket
    if (socket == null) {
      Log.w(TAG, "cannot send action '${action.key}', ws is null")
      return false
    }
    val request = WsRequest(action.key, value)
    val jsonString = jsonParser.toJson(request)

    Log.d(TAG, "sending action: $jsonString")
    return socket.send(jsonString)
  }

  private fun resetWatchdog() {
    watchdogJob?.cancel()
    watchdogJob = watchdogScope?.launch {
      delay(HEARTBEAT_TIMEOUT_MS)
      Log.w(TAG, "watchdog timeout, no message received for $HEARTBEAT_TIMEOUT_MS ms, cancelling ws")
      webSocket?.cancel()
    }
  }

  private fun stopWatchdog() {
    watchdogJob?.cancel()
    watchdogScope?.cancel()
    watchdogJob = null
    watchdogScope = null
  }
}
