package pl.miloszgilga.htas.client.composable.settings

import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.size
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Visibility
import androidx.compose.material.icons.filled.VisibilityOff
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.composable.row.RowBase

@Composable
fun PasswordSettingsRow(
  label: String,
  password: String,
) {
  var isPasswordVisible by remember { mutableStateOf(false) }

  RowBase(label) {
    Row(verticalAlignment = Alignment.CenterVertically) {
      Text(
        text = if (isPasswordVisible) {
          password
        } else {
          "••••••••"
        },
        style = MaterialTheme.typography.bodyLarge,
        fontWeight = FontWeight.SemiBold,
        color = MaterialTheme.colorScheme.onSurface,
      )
      IconButton(onClick = { isPasswordVisible = !isPasswordVisible }) {
        Icon(
          imageVector = if (isPasswordVisible) {
            Icons.Default.VisibilityOff
          } else {
            Icons.Default.Visibility
          },
          contentDescription = null,
          modifier = Modifier.size(20.dp),
          tint = MaterialTheme.colorScheme.primary,
        )
      }
    }
  }
}
