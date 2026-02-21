package pl.miloszgilga.htas.client.composable

import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp

@Composable
fun SectionHeader(
  text: String,
  modifier: Modifier = Modifier,
  color: Color = MaterialTheme.colorScheme.primary,
) {
  Text(
    text = text,
    style = MaterialTheme.typography.labelMedium,
    color = color,
    modifier = modifier
      .fillMaxWidth()
      .padding(top = 16.dp, start = 16.dp),
  )
}
