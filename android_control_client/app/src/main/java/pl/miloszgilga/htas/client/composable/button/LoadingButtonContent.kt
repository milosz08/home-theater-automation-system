package pl.miloszgilga.htas.client.composable.button

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.size
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp

@Composable
fun LoadingButtonContent(
  text: String,
  isLoading: Boolean,
  modifier: Modifier = Modifier,
  progressColor: Color = MaterialTheme.colorScheme.onPrimary,
) {
  Box(
    contentAlignment = Alignment.Center,
    modifier = modifier
  ) {
    Text(
      text = text,
      modifier = Modifier.alpha(if (isLoading) 0f else 1f)
    )
    if (isLoading) {
      CircularProgressIndicator(
        modifier = Modifier.size(20.dp),
        color = progressColor,
        strokeWidth = 2.dp,
      )
    }
  }
}
