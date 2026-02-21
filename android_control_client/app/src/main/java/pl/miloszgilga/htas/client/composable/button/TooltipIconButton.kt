package pl.miloszgilga.htas.client.composable.button

import androidx.compose.foundation.layout.Box
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.PlainTooltip
import androidx.compose.material3.Text
import androidx.compose.material3.TooltipAnchorPosition
import androidx.compose.material3.TooltipBox
import androidx.compose.material3.TooltipDefaults
import androidx.compose.material3.rememberTooltipState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.unit.dp

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TooltipIconButton(
  icon: ImageVector,
  contentDescription: String,
  onClick: () -> Unit,
  modifier: Modifier = Modifier,
  enabled: Boolean = true,
  disabledMessage: String? = null,
) {
  val tooltipState = rememberTooltipState()

  TooltipBox(
    positionProvider = TooltipDefaults.rememberTooltipPositionProvider(
      TooltipAnchorPosition.Above,
      spacingBetweenTooltipAndAnchor = 8.dp,
    ),
    tooltip = {
      PlainTooltip {
        val textToShow = if (!enabled && disabledMessage != null) {
          disabledMessage
        } else {
          contentDescription
        }
        Text(textToShow)
      }
    },
    state = tooltipState,
  ) {
    Box(modifier = modifier) {
      IconButton(
        onClick = onClick,
        enabled = enabled,
      ) {
        Icon(
          imageVector = icon,
          contentDescription = contentDescription,
        )
      }
      if (!enabled) {
        Box(
          modifier = Modifier.matchParentSize(),
        )
      }
    }
  }
}
