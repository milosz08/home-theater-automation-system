package pl.miloszgilga.htas.client.util

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.Immutable
import androidx.compose.runtime.ReadOnlyComposable
import androidx.compose.runtime.staticCompositionLocalOf
import androidx.compose.ui.graphics.Color

@Immutable
data class AppColors(
  val success: Color = Color(0xFF4CAF50),
  val temperature: Color = Color(0xFFFF5252),
  val humidity: Color = Color(0xFF2196F3),
  val pressure: Color = Color(0xFF4CAF50),
  val ramUsage: Color = Color(0xFFFFC107),
)

val LocalAppColors = staticCompositionLocalOf { AppColors() }

@Suppress("UnusedReceiverParameter")
val MaterialTheme.appColors: AppColors
  @Composable
  @ReadOnlyComposable
  get() = LocalAppColors.current

@Composable
fun HTASClientTheme(content: @Composable () -> Unit) {
  val colors = AppColors()

  CompositionLocalProvider(LocalAppColors provides colors) {
    MaterialTheme(
      colorScheme = darkColorScheme(),
      content = content,
    )
  }
}
