package pl.miloszgilga.htas.client.composable.settings

import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun FooterText(
  label: String,
  value: String,
) {
  Text(
    text = "$label: $value",
    style = MaterialTheme.typography.labelSmall,
    color = MaterialTheme.colorScheme.outline,
    modifier = Modifier.padding(vertical = 8.dp),
  )
}
