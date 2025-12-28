package pl.miloszgilga.hometheatercontroller.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Typography
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable

@Composable
fun HomeTheaterControllerTheme(content: @Composable () -> Unit) {
  MaterialTheme(
    colorScheme = darkColorScheme(
      primary = AppPrimary,
      onPrimary = TextWhite,
      background = AppBackground,
      onBackground = TextWhite,
      surface = SurfaceGrey,
      onSurface = TextWhite,
      secondary = TextGrey,
      onSecondary = AppBackground,
      error = Error,
      onError = TextWhite,
    ),
    typography = Typography(),
    content = content
  )
}
