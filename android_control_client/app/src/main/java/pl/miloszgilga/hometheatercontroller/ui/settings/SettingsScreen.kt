package pl.miloszgilga.hometheatercontroller.ui.settings

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.KeyboardType
import pl.miloszgilga.hometheatercontroller.store.AppUiState
import pl.miloszgilga.hometheatercontroller.store.ConnectionStatus
import pl.miloszgilga.hometheatercontroller.ui.common.ConnectionStatus

@Composable
fun SettingsScreen(
  state: AppUiState,
  onConnectionSettingsChanged: (String, String) -> Unit,
) {
  Column(modifier = Modifier.fillMaxSize()) {
    SettingSection(title = "Connection settings") {
      SettingInputItem(
        label = "(ESP32) IP address or hostname (supports local DNS)",
        value = state.ipAddress,
        // disable click when app is connecting with ESP32
        enabled = state.connectionStatus != ConnectionStatus.CONNECTING,
        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Uri),
        validator = { it.isNotBlank() && !it.contains(" ") },
        errorMessage = "Invalid address format (no spaces allowed)!",
        onConfirm = { onConnectionSettingsChanged(it, state.restApiPort) },
      )
      SettingInputItem(
        label = "(ESP32) Rest API server port",
        value = state.restApiPort,
        enabled = state.connectionStatus != ConnectionStatus.CONNECTING,
        keyboardOptions = KeyboardOptions(
          keyboardType = KeyboardType.Number
        ),
        validator = {
          val port = it.toIntOrNull()
          port != null && port in 1..65535
        },
        errorMessage = "Port must be between 1 and 65535!",
        onConfirm = { onConnectionSettingsChanged(state.ipAddress, it) }
      )
      ConnectionStatus(connectionStatus = state.connectionStatus)
    }
  }
}
