package pl.miloszgilga.hometheatercontroller.ui.common

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import pl.miloszgilga.hometheatercontroller.store.ConnectionStatus

@Composable
fun ConnectionStatus(connectionStatus: ConnectionStatus) {
  Box(modifier = Modifier.padding(16.dp)) {
    Row {
      Text(
        text = "Connection status: ",
        modifier = Modifier.alignByBaseline()
      )
      Text(
        text = connectionStatus.name,
        color = connectionStatus.color,
        modifier = Modifier.alignByBaseline()
      )
    }
  }
}
