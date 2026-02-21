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
fun ResponsiveDualActionLayout(
  modifier: Modifier = Modifier,
  primaryAction: @Composable (Modifier) -> Unit,
  secondaryAction: @Composable (Modifier) -> Unit,
) {
  val configuration = LocalConfiguration.current
  val isLandscape = configuration.orientation == Configuration.ORIENTATION_LANDSCAPE

  val updatedModifier = modifier
    .fillMaxWidth()
    .padding(vertical = 16.dp)

  if (isLandscape) {
    Row(
      modifier = updatedModifier,
      horizontalArrangement = Arrangement.spacedBy(12.dp),
    ) {
      primaryAction(Modifier.weight(1f))
      secondaryAction(Modifier.weight(1f))
    }
  } else {
    Column(
      modifier = updatedModifier,
      verticalArrangement = Arrangement.spacedBy(12.dp),
    ) {
      primaryAction(Modifier.fillMaxWidth())
      secondaryAction(Modifier.fillMaxWidth())
    }
  }
}
