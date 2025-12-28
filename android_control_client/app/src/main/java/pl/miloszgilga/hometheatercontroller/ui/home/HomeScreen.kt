package pl.miloszgilga.hometheatercontroller.ui.home

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import pl.miloszgilga.hometheatercontroller.store.AppUiState
import pl.miloszgilga.hometheatercontroller.store.ConnectionStatus
import pl.miloszgilga.hometheatercontroller.ui.common.ConnectionStatus

@Composable
fun HomeScreen(
  state: AppUiState,
  onRetry: () -> Unit,
) {
  Column(
    modifier = Modifier
      .fillMaxSize()
      .padding(24.dp),
    horizontalAlignment = Alignment.CenterHorizontally,
    verticalArrangement = if (state.connectionStatus == ConnectionStatus.CONNECTED) {
      Arrangement.Top
    } else {
      Arrangement.Center
    },
  ) {
    when (state.connectionStatus) {
      ConnectionStatus.CONNECTED -> {
        ConnectionStatus(connectionStatus = state.connectionStatus)
        Controls(state = state)
      }

      ConnectionStatus.CONNECTING -> {
        CircularProgressIndicator()
        Spacer(modifier = Modifier.height(16.dp))
        Text("Connecting with remote controller...")
      }

      else -> {
        Icon(
          imageVector = Icons.Filled.Warning,
          contentDescription = "Error",
          tint = MaterialTheme.colorScheme.error,
          modifier = Modifier.size(48.dp)
        )
        Spacer(modifier = Modifier.height(16.dp))
        Text(
          text = "No connection with remote controller!",
          style = MaterialTheme.typography.titleMedium,
        )
        Text(
          text = "${state.ipAddress}:${state.restApiPort}",
          style = MaterialTheme.typography.bodySmall,
        )
        Spacer(modifier = Modifier.height(24.dp))
        Button(onClick = onRetry) {
          Text("Try again")
        }
      }
    }
  }
}
