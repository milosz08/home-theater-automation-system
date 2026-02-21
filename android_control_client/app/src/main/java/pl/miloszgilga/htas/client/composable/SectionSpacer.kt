package pl.miloszgilga.htas.client.composable

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.width
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

enum class SpacerOrientation {
  VERTICAL,
  HORIZONTAL,
  ;
}

enum class SpacerSize {
  NORMAL,
  SMALL,
  LARGE,
  ;
}

@Composable
fun SectionSpacer(
  modifier: Modifier = Modifier,
  orientation: SpacerOrientation = SpacerOrientation.VERTICAL,
  size: SpacerSize = SpacerSize.NORMAL,
) {
  val sizeDp = when (size) {
    SpacerSize.SMALL -> 8
    SpacerSize.NORMAL -> 16
    SpacerSize.LARGE -> 40
  }.dp

  Spacer(
    modifier = if (orientation == SpacerOrientation.VERTICAL) {
      modifier.height(sizeDp)
    } else {
      modifier.width(sizeDp)
    }
  )
}
