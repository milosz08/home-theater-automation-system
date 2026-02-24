package pl.miloszgilga.htas.client.composable.main

import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.SystemUpdate
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ProgressIndicatorDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.AnimatedProgressBar
import pl.miloszgilga.htas.client.composable.SectionSpacer
import pl.miloszgilga.htas.client.composable.SpacerSize
import pl.miloszgilga.htas.client.util.UiText

@Composable
fun FirmwareUpdateSection(
  statusText: UiText?,
  progressPercent: Int,
) {
  val context = LocalContext.current
  val animatedProgress by animateFloatAsState(
    targetValue = progressPercent / 100f,
    animationSpec = ProgressIndicatorDefaults.ProgressAnimationSpec,
    label = "ota_progress_animation",
  )
  val isProgressVisible = progressPercent in 1..99

  Column(
    modifier = Modifier
      .fillMaxSize()
      .padding(32.dp),
    horizontalAlignment = Alignment.CenterHorizontally,
    verticalArrangement = Arrangement.Center,
  ) {
    Box(contentAlignment = Alignment.Center) {
      CircularProgressIndicator(
        modifier = Modifier.size(72.dp),
        color = MaterialTheme.colorScheme.primary,
        strokeWidth = 4.dp,
      )
      Icon(
        imageVector = Icons.Default.SystemUpdate,
        contentDescription = null,
        modifier = Modifier.size(32.dp),
        tint = MaterialTheme.colorScheme.primary.copy(alpha = 0.8f),
      )
    }
    SectionSpacer(size = SpacerSize.NORMAL)
    Text(
      text = stringResource(R.string.firmware_updating),
      style = MaterialTheme.typography.headlineSmall,
      color = MaterialTheme.colorScheme.onSurface,
      fontWeight = FontWeight.Bold,
    )
    SectionSpacer(size = SpacerSize.SMALL)
    Text(
      text = "${statusText?.asString(context) ?: stringResource(R.string.please_wait)}...",
      style = MaterialTheme.typography.bodyLarge,
      color = MaterialTheme.colorScheme.onSurfaceVariant,
      textAlign = TextAlign.Center,
    )
    SectionSpacer(size = SpacerSize.LARGE)
    Box(
      modifier = Modifier
        .fillMaxWidth()
        .height(48.dp),
      contentAlignment = Alignment.Center,
    ) {
      Column(horizontalAlignment = Alignment.CenterHorizontally) {
        AnimatedProgressBar(
          isVisible = isProgressVisible,
          progress = animatedProgress,
        )
        if (isProgressVisible) {
          SectionSpacer(size = SpacerSize.NORMAL)
          Text(
            text = "$progressPercent%",
            style = MaterialTheme.typography.titleMedium,
            color = MaterialTheme.colorScheme.onSurface,
            fontWeight = FontWeight.SemiBold,
          )
        }
      }
    }
  }
}
