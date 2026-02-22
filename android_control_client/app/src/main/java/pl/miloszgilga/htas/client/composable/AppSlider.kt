package pl.miloszgilga.htas.client.composable

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun AppSlider(
  label: String,
  formattedValue: String,
  value: Float,
  valueRange: ClosedFloatingPointRange<Float>,
  modifier: Modifier = Modifier,
  enabled: Boolean = true,
  steps: Int = 0,
  onValueChange: (Float) -> Unit,
) {
  Column(
    modifier = modifier
      .fillMaxWidth()
      .padding(vertical = 16.dp)
  ) {
    Row(
      modifier = Modifier.fillMaxWidth(),
      horizontalArrangement = Arrangement.SpaceBetween
    ) {
      Text(
        text = label,
        style = MaterialTheme.typography.bodyMedium,
        color = if (enabled) {
          MaterialTheme.colorScheme.onSurface
        } else {
          MaterialTheme.colorScheme.onSurface.copy(alpha = 0.38f)
        },
      )
      Text(
        text = formattedValue,
        color = if (enabled) {
          MaterialTheme.colorScheme.primary
        } else {
          MaterialTheme.colorScheme.onSurface.copy(alpha = 0.38f)
        },
        style = MaterialTheme.typography.bodyMedium,
      )
    }
    Slider(
      value = value,
      onValueChange = onValueChange,
      valueRange = valueRange,
      steps = steps,
      enabled = enabled,
    )
  }
}
