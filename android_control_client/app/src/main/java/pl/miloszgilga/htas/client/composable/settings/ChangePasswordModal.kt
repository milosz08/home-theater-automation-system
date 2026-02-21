package pl.miloszgilga.htas.client.composable.settings

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.PasswordInputField
import pl.miloszgilga.htas.client.composable.button.AppButton
import pl.miloszgilga.htas.client.composable.button.AppTextButton

@Composable
fun ChangePasswordModal(
  onDismiss: () -> Unit,
  onConfirm: suspend (oldPas: String, newPass: String) -> Unit,
) {
  var oldPass by remember { mutableStateOf("") }
  var newPass by remember { mutableStateOf("") }
  var confirmPass by remember { mutableStateOf("") }

  val passwordsMatch = newPass == confirmPass
  val isFormValid =
    oldPass.isNotBlank() && newPass.isNotBlank() && confirmPass.isNotBlank() && passwordsMatch

  AlertDialog(
    onDismissRequest = onDismiss,
    title = {
      Text(stringResource(R.string.change_password))
    },
    text = {
      Column(
        verticalArrangement = Arrangement.spacedBy(8.dp),
        modifier = Modifier.fillMaxWidth(),
      ) {
        PasswordInputField(
          value = oldPass,
          onValueChange = { oldPass = it },
          label = stringResource(R.string.current_password)
        )
        PasswordInputField(
          value = newPass,
          onValueChange = { newPass = it },
          label = stringResource(R.string.new_password)
        )
        PasswordInputField(
          value = confirmPass,
          onValueChange = { confirmPass = it },
          label = stringResource(R.string.confirm_new_password),
          isError = confirmPass.isNotBlank() && !passwordsMatch,
        )
        if (confirmPass.isNotBlank() && !passwordsMatch) {
          Text(
            text = stringResource(R.string.passwords_are_not_the_same),
            color = MaterialTheme.colorScheme.error,
            style = MaterialTheme.typography.bodySmall,
            modifier = Modifier.padding(start = 16.dp, top = 2.dp),
          )
        }
      }
    },
    confirmButton = {
      AppButton(
        text = stringResource(R.string.save),
        onClick = {
          if (isFormValid) {
            onConfirm(oldPass, newPass)
          }
        },
      )
    },
    dismissButton = {
      AppTextButton(
        text = stringResource(R.string.cancel),
        onClick = onDismiss,
      )
    }
  )
}
