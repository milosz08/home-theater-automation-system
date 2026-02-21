package pl.miloszgilga.htas.client.composable.settings

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.row.RowBase
import pl.miloszgilga.htas.client.util.appColors

@Composable
fun ConnectionStatusBadgeRow(isConnected: Boolean) {
  val statusColor = if (isConnected) {
    MaterialTheme.appColors.success
  } else {
    MaterialTheme.colorScheme.error
  }

  RowBase(stringResource(R.string.connection_status)) {
    Surface(
      color = statusColor.copy(alpha = 0.1f),
      shape = RoundedCornerShape(16.dp),
      border = BorderStroke(1.dp, statusColor.copy(alpha = 0.5f))
    ) {
      Row(
        modifier = Modifier.padding(horizontal = 12.dp, vertical = 6.dp),
        verticalAlignment = Alignment.CenterVertically,
      ) {
        Box(
          modifier = Modifier
            .size(8.dp)
            .background(statusColor, CircleShape),
        )
        Spacer(modifier = Modifier.width(8.dp))
        Text(
          text = stringResource(
            if (isConnected) {
              R.string.connected
            } else {
              R.string.disconnected
            }
          ),
          style = MaterialTheme.typography.labelLarge,
          color = statusColor,
          fontWeight = FontWeight.Bold
        )
      }
    }
  }
}
