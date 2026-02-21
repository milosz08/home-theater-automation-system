package pl.miloszgilga.htas.client.composable.responsive

import android.content.res.Configuration
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp

@Composable
fun ResponsiveLayout(
  modifier: Modifier = Modifier,
  spacing: Dp = 16.dp,
  content: @Composable (isPortrait: Boolean) -> Unit,
) {
  val configuration = LocalConfiguration.current

  if (configuration.orientation == Configuration.ORIENTATION_PORTRAIT) {
    Column(
      modifier = modifier,
      verticalArrangement = Arrangement.spacedBy(spacing),
      horizontalAlignment = Alignment.CenterHorizontally,
    ) {
      content(true)
    }
  } else {
    Row(
      modifier = modifier,
      horizontalArrangement = Arrangement.spacedBy(spacing, Alignment.CenterHorizontally),
      verticalAlignment = Alignment.CenterVertically,
    ) {
      content(false)
    }
  }
}
