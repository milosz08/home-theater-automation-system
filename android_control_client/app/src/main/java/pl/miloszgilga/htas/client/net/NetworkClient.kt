package pl.miloszgilga.htas.client.net

abstract class NetworkClient {
  companion object {
    @PublishedApi
    internal const val AUTH_HEADER_NAME = "x-auth-pass"
  }
}
