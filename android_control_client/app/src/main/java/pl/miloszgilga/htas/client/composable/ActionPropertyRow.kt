package pl.miloszgilga.htas.client.composable

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.composable.row.RowBase

@Composable
fun ActionPropertyRow(
  label: String,
  value: String,
  icon: ImageVector,
  onClick: () -> Unit,
  modifier: Modifier = Modifier,
  contentDescription: String? = null,
  fontWeight: FontWeight = FontWeight.Normal,
) {
  RowBase(label, modifier = modifier) {
    Row(
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.spacedBy(8.dp),
    ) {
      Text(
        text = value,
        style = MaterialTheme.typography.bodyLarge,
        fontWeight = fontWeight,
        color = MaterialTheme.colorScheme.onSurface,
      )
      IconButton(
        onClick = onClick,
        modifier = Modifier.size(24.dp),
      ) {
        Icon(
          imageVector = icon,
          contentDescription = contentDescription,
          tint = MaterialTheme.colorScheme.primary,
          modifier = Modifier.size(20.dp),
        )
      }
    }
  }
}
