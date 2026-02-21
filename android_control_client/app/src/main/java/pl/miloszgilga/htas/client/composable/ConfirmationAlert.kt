package pl.miloszgilga.htas.client.composable

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.res.stringResource
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.button.AppButton
import pl.miloszgilga.htas.client.composable.button.AppTextButton
import pl.miloszgilga.htas.client.composable.button.ButtonType

@Composable
fun ConfirmationAlert(
  title: String,
  text: String,
  confirmText: String,
  onConfirm: suspend () -> Unit,
  onDismiss: () -> Unit,
  dismissText: String = stringResource(R.string.cancel),
  isSolidConfirmButton: Boolean = false,
) {
  AlertDialog(
    onDismissRequest = onDismiss,
    title = { Text(title) },
    text = { Text(text) },
    confirmButton = {
      if (isSolidConfirmButton) {
        AppButton(
          text = confirmText,
          type = ButtonType.ERROR,
          onClick = onConfirm,
        )
      } else {
        AppTextButton(
          text = confirmText,
          type = ButtonType.ERROR,
          onClick = onConfirm,
        )
      }
    },
    dismissButton = {
      TextButton(onClick = onDismiss) {
        Text(dismissText)
      }
    },
  )
}
