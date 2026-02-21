package pl.miloszgilga.htas.client.net

sealed class NetworkEvent {
  object Connected : NetworkEvent()
  data class Disconnected(val reason: String?) : NetworkEvent()
  data class Error(val throwable: Throwable) : NetworkEvent()
  data class MessageReceived(val text: String) : NetworkEvent()
}
