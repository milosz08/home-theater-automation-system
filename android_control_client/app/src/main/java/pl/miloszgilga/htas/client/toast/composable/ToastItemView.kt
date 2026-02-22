package pl.miloszgilga.htas.client.toast.composable

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.slideInVertically
import androidx.compose.animation.slideOutVertically
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.delay
import pl.miloszgilga.htas.client.toast.ToastManager
import pl.miloszgilga.htas.client.toast.ToastModel
import pl.miloszgilga.htas.client.toast.ToastType

@Composable
fun ToastItemView(toast: ToastModel) {
  var isVisible by remember { mutableStateOf(false) }

  LaunchedEffect(toast.id) {
    isVisible = true
    delay(3000)
    isVisible = false
    delay(300)
    ToastManager.dismiss(toast.id)
  }

  AnimatedVisibility(
    visible = isVisible,
    enter = slideInVertically { it } + fadeIn(),
    exit = slideOutVertically { it } + fadeOut(),
  ) {
    Surface(
      color = when (toast.type) {
        ToastType.NORMAL -> MaterialTheme.colorScheme.inverseSurface
        ToastType.ERROR -> MaterialTheme.colorScheme.errorContainer
      },
      contentColor = when (toast.type) {
        ToastType.NORMAL -> MaterialTheme.colorScheme.inverseOnSurface
        ToastType.ERROR -> MaterialTheme.colorScheme.onErrorContainer
      },
      shape = RoundedCornerShape(50),
      shadowElevation = 4.dp,
      modifier = Modifier.padding(horizontal = 16.dp),
    ) {
      Row(
        modifier = Modifier.padding(horizontal = 24.dp, vertical = 12.dp),
        verticalAlignment = Alignment.CenterVertically,
      ) {
        Text(
          text = toast.message,
          style = MaterialTheme.typography.bodyMedium,
        )
      }
    }
  }
}
