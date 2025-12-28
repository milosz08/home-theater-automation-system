package pl.miloszgilga.hometheatercontroller.store

data class AppUiState(
  val ipAddress: String = "192.168.0.2",
  val restApiPort: String = "8080",
  val connectionStatus: ConnectionStatus = ConnectionStatus.CONNECTING,
  val isAppLocked: Boolean = false,
)
