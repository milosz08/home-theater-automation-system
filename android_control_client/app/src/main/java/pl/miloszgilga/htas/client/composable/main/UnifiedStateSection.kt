package pl.miloszgilga.htas.client.composable.main

import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.composable.CenteredScreenWrapper
import pl.miloszgilga.htas.client.composable.SectionSpacer
import pl.miloszgilga.htas.client.composable.SpacerSize
import pl.miloszgilga.htas.client.composable.button.AppButton
import pl.miloszgilga.htas.client.composable.button.AppOutlinedButton
import pl.miloszgilga.htas.client.composable.button.ButtonType
import pl.miloszgilga.htas.client.composable.responsive.ResponsiveLayout

enum class StateMessageType {
  NORMAL,
  ERROR,
  ;
}

@Composable
fun UnifiedStateSection(
  icon: ImageVector,
  title: String,
  description: String? = null,
  primaryButtonText: String,
  onPrimaryClick: suspend () -> Unit,
  secondaryButtonText: String? = null,
  onSecondaryClick: suspend () -> Unit = {},
  type: StateMessageType = StateMessageType.NORMAL,
) {
  val buttonType = when (type) {
    StateMessageType.NORMAL -> ButtonType.NORMAL
    StateMessageType.ERROR -> ButtonType.ERROR
  }

  CenteredScreenWrapper {
    Icon(
      imageVector = icon,
      contentDescription = null,
      modifier = Modifier.size(64.dp),
      tint = when (type) {
        StateMessageType.NORMAL -> MaterialTheme.colorScheme.outline
        StateMessageType.ERROR -> MaterialTheme.colorScheme.error
      },
    )
    SectionSpacer()
    Text(
      text = title,
      style = MaterialTheme.typography.headlineSmall,
      color = when (type) {
        StateMessageType.NORMAL -> MaterialTheme.colorScheme.onSurface
        StateMessageType.ERROR -> MaterialTheme.colorScheme.error
      },
      textAlign = TextAlign.Center,
    )
    if (description != null) {
      SectionSpacer(size = SpacerSize.SMALL)
      Text(
        text = description,
        style = MaterialTheme.typography.bodyMedium,
        color = MaterialTheme.colorScheme.onSurfaceVariant,
        textAlign = TextAlign.Center,
        modifier = Modifier.padding(horizontal = 16.dp),
      )
    }
    SectionSpacer(size = SpacerSize.LARGE)
    ResponsiveLayout(spacing = 12.dp) { isPortrait ->
      val buttonModifier = Modifier
        .then(if (isPortrait) Modifier.fillMaxWidth() else Modifier)
      AppButton(
        text = primaryButtonText,
        onClick = onPrimaryClick,
        modifier = buttonModifier,
        type = buttonType
      )

      if (secondaryButtonText != null) {
        AppOutlinedButton(
          text = secondaryButtonText,
          onClick = onSecondaryClick,
          modifier = buttonModifier,
          type = buttonType
        )
      }
    }
  }
}
