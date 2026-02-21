package pl.miloszgilga.htas.client.composable.button

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.size
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.composable.SectionSpacer
import pl.miloszgilga.htas.client.composable.SpacerOrientation
import pl.miloszgilga.htas.client.composable.SpacerSize

@Composable
fun LoadingButtonContent(
  text: String,
  isLoading: Boolean,
  modifier: Modifier = Modifier,
  progressColor: Color = MaterialTheme.colorScheme.onPrimary,
  icon: ImageVector? = null,
) {
  Box(
    contentAlignment = Alignment.Center,
    modifier = modifier,
  ) {
    Row(
      modifier = Modifier.alpha(if (isLoading) 0f else 1f),
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.Center,
    ) {
      if (icon != null) {
        Icon(
          imageVector = icon,
          contentDescription = null,
          modifier = Modifier.size(20.dp),
        )
        SectionSpacer(
          orientation = SpacerOrientation.HORIZONTAL,
          size = SpacerSize.SMALL,
        )
      }
      Text(text)
    }
    if (isLoading) {
      CircularProgressIndicator(
        modifier = Modifier.size(20.dp),
        color = progressColor,
        strokeWidth = 2.dp,
      )
    }
  }
}
