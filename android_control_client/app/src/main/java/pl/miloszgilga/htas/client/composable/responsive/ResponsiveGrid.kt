package pl.miloszgilga.htas.client.composable.responsive

import android.content.res.Configuration
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp

@Composable
fun ResponsiveGrid(
  items: List<@Composable () -> Unit>,
  modifier: Modifier = Modifier,
  spacing: Dp = 16.dp,
) {
  val configuration = LocalConfiguration.current
  val isPortrait = configuration.orientation == Configuration.ORIENTATION_PORTRAIT

  Column(
    modifier = modifier,
    verticalArrangement = Arrangement.spacedBy(spacing),
  ) {
    if (isPortrait) {
      items.forEach { it() }
    } else {
      items.chunked(2).forEach { rowItems ->
        Row(horizontalArrangement = Arrangement.spacedBy(spacing)) {
          rowItems.forEach { item ->
            Box(modifier = Modifier.weight(1f)) { item() }
          }
          if (rowItems.size == 1) {
            Spacer(modifier = Modifier.weight(1f))
          }
        }
      }
    }
  }
}
