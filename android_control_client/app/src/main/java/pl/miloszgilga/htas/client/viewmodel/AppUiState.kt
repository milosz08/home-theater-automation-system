package pl.miloszgilga.htas.client.viewmodel

import pl.miloszgilga.htas.client.store.ServerConfig
import pl.miloszgilga.htas.client.util.UiText

sealed class AppUiState {
  object Loading : AppUiState()
  object Unpaired : AppUiState()
  object Connecting : AppUiState()
  data class QrScanError(val error: UiText) : AppUiState()
  data class Disconnected(val config: ServerConfig) : AppUiState()
  data class Connected(val config: ServerConfig) : AppUiState()
  data class ConnectionFailed(val config: ServerConfig, val error: UiText) : AppUiState()
}
