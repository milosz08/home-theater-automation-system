package pl.miloszgilga.htas.client.composable.row

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight

@Composable
fun TextPropertyRow(
  label: String,
  value: String,
  modifier: Modifier = Modifier,
) {
  RowBase(
    label = label,
    modifier = modifier,
  ) {
    Text(
      text = value,
      style = MaterialTheme.typography.bodyLarge,
      fontWeight = FontWeight.SemiBold,
      color = MaterialTheme.colorScheme.onSurface,
    )
  }
}
