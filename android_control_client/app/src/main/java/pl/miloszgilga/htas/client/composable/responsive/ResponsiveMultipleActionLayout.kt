package pl.miloszgilga.htas.client.composable.responsive

import android.content.res.Configuration
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.unit.dp

@Composable
fun ResponsiveMultipleActionLayout(
  modifier: Modifier = Modifier,
  includeVerticalSpace: Boolean = true,
  includeHorizontalSpace: Boolean = true,
  actions: List<@Composable (Modifier) -> Unit>,
) {
  val configuration = LocalConfiguration.current
  val isLandscape = configuration.orientation == Configuration.ORIENTATION_LANDSCAPE

  val updatedModifier = modifier
    .fillMaxWidth()
    .padding(
      vertical = if (includeVerticalSpace) 16.dp else 0.dp,
      horizontal = if (includeHorizontalSpace) 16.dp else 0.dp,
    )

  if (actions.isEmpty()) {
    return
  }
  if (isLandscape) {
    Row(
      modifier = updatedModifier,
      horizontalArrangement = Arrangement.spacedBy(12.dp),
    ) {
      actions.forEach { action ->
        action(Modifier.weight(1f))
      }
    }
  } else {
    Column(
      modifier = updatedModifier,
      verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
      actions.forEach { action ->
        action(Modifier.fillMaxWidth())
      }
    }
  }
}
