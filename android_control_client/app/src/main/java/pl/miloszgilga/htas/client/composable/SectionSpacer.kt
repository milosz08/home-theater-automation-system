package pl.miloszgilga.htas.client.composable

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

enum class SpacerSize {
  NORMAL,
  SMALL,
  LARGE,
  ;
}

@Composable
fun SectionSpacer(
  modifier: Modifier = Modifier,
  spacerSize: SpacerSize = SpacerSize.NORMAL,
) {
  Spacer(
    modifier = modifier.height(
      when (spacerSize) {
        SpacerSize.SMALL -> 8
        SpacerSize.NORMAL -> 16
        SpacerSize.LARGE -> 40
      }.dp,
    )
  )
}
