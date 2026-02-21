package pl.miloszgilga.htas.client.composable.button

import androidx.compose.foundation.BorderStroke
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch

enum class ButtonType {
  NORMAL,
  ERROR,
  ;
}

@Composable
fun AppButton(
  text: String,
  onClick: suspend () -> Unit,
  modifier: Modifier = Modifier,
  enabled: Boolean = true,
  icon: ImageVector? = null,
  type: ButtonType = ButtonType.NORMAL,
) {
  var isLoading by remember { mutableStateOf(false) }
  val scope = rememberCoroutineScope()

  Button(
    onClick = {
      if (!isLoading) {
        scope.launch {
          isLoading = true
          try {
            onClick()
          } finally {
            isLoading = false
          }
        }
      }
    },
    modifier = modifier,
    enabled = enabled,
    colors = ButtonDefaults.buttonColors(
      containerColor = when (type) {
        ButtonType.NORMAL -> MaterialTheme.colorScheme.primary
        ButtonType.ERROR -> MaterialTheme.colorScheme.error
      },
      contentColor = when (type) {
        ButtonType.NORMAL -> MaterialTheme.colorScheme.onPrimary
        ButtonType.ERROR -> MaterialTheme.colorScheme.onError
      },
      disabledContainerColor = MaterialTheme.colorScheme.error.copy(alpha = 0.12f),
      disabledContentColor = MaterialTheme.colorScheme.error.copy(alpha = 0.38f),
    )
  ) {
    LoadingButtonContent(
      text = text,
      icon = icon,
      isLoading = isLoading,
      progressColor = if (type == ButtonType.ERROR) {
        MaterialTheme.colorScheme.onError
      } else {
        MaterialTheme.colorScheme.onPrimary
      },
    )
  }
}

@Composable
fun AppOutlinedButton(
  text: String,
  onClick: suspend () -> Unit,
  modifier: Modifier = Modifier,
  enabled: Boolean = true,
  icon: ImageVector? = null,
  type: ButtonType = ButtonType.NORMAL,
) {
  var isLoading by remember { mutableStateOf(false) }
  val scope = rememberCoroutineScope()

  val baseColor = when (type) {
    ButtonType.NORMAL -> MaterialTheme.colorScheme.primary
    ButtonType.ERROR -> MaterialTheme.colorScheme.error
  }

  OutlinedButton(
    onClick = {
      if (!isLoading) {
        scope.launch {
          isLoading = true
          try {
            onClick()
          } finally {
            isLoading = false
          }
        }
      }
    },
    modifier = modifier,
    enabled = enabled,
    colors = ButtonDefaults.outlinedButtonColors(
      contentColor = baseColor,
      disabledContentColor = baseColor.copy(alpha = 0.38f),
    ),
    border = BorderStroke(
      width = 1.dp,
      color = if (enabled) {
        baseColor
      } else {
        baseColor.copy(alpha = 0.12f)
      },
    )
  ) {
    LoadingButtonContent(
      text = text,
      icon = icon,
      isLoading = isLoading,
      progressColor = baseColor,
    )
  }
}

@Composable
fun AppTextButton(
  text: String,
  onClick: suspend () -> Unit,
  modifier: Modifier = Modifier,
  enabled: Boolean = true,
  icon: ImageVector? = null,
  type: ButtonType = ButtonType.NORMAL,
) {
  var isLoading by remember { mutableStateOf(false) }
  val scope = rememberCoroutineScope()

  val color = when (type) {
    ButtonType.NORMAL -> MaterialTheme.colorScheme.primary
    ButtonType.ERROR -> MaterialTheme.colorScheme.error
  }

  TextButton(
    onClick = {
      if (!isLoading) {
        scope.launch {
          isLoading = true
          try {
            onClick()
          } finally {
            isLoading = false
          }
        }
      }
    },
    modifier = modifier,
    enabled = enabled,
    colors = ButtonDefaults.textButtonColors(contentColor = color)
  ) {
    LoadingButtonContent(
      text = text,
      icon = icon,
      isLoading = isLoading,
      progressColor = color,
    )
  }
}
