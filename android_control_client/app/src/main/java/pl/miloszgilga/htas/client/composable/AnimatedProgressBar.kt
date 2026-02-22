package pl.miloszgilga.htas.client.composable

import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.unit.dp

@Composable
fun AnimatedProgressBar(
  isVisible: Boolean,
  progress: Float,
  modifier: Modifier = Modifier,
) {
  val progressAlpha by animateFloatAsState(
    targetValue = if (isVisible) 1f else 0f,
    label = "progress_alpha_animation"
  )

  Box(
    modifier = modifier
      .fillMaxWidth()
      .height(4.dp)
      .alpha(progressAlpha)
      .background(MaterialTheme.colorScheme.surfaceVariant)
  ) {
    Box(
      modifier = Modifier
        .fillMaxHeight()
        .fillMaxWidth(fraction = progress.coerceIn(0f, 1f))
        .background(MaterialTheme.colorScheme.primary)
    )
  }
}
