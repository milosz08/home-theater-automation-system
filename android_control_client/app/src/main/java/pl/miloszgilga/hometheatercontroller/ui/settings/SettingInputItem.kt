package pl.miloszgilga.hometheatercontroller.ui.settings

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun SettingInputItem(
  label: String,
  value: String,
  enabled: Boolean = true,
  keyboardOptions: KeyboardOptions = KeyboardOptions.Default,
  validator: (String) -> Boolean = { it.trim().isNotEmpty() },
  errorMessage: String = "Value cannot be empty",
  onConfirm: (value: String) -> Unit,
) {
  var showDialog by remember { mutableStateOf(false) }
  var text by remember { mutableStateOf(value) }
  val isValid = validator(text)

  Column(
    modifier = Modifier
      .fillMaxWidth()
      .clickable(
        enabled = enabled,
        onClick = {
          text = value
          showDialog = true
        })
      .padding(vertical = 4.dp)
  ) {
    Column(
      modifier = Modifier.padding(horizontal = 16.dp, vertical = 12.dp)
    ) {
      Text(
        text = label,
        style = MaterialTheme.typography.bodyMedium,
        color = MaterialTheme.colorScheme.onSurface
      )
      Spacer(modifier = Modifier.height(4.dp))
      Text(
        text = value,
        style = MaterialTheme.typography.bodySmall,
        color = MaterialTheme.colorScheme.onSurfaceVariant
      )
    }
    HorizontalDivider(
      modifier = Modifier.padding(horizontal = 16.dp),
      thickness = 0.5.dp,
      color = MaterialTheme.colorScheme.outlineVariant
    )
  }
  if (showDialog) {
    AlertDialog(
      onDismissRequest = { showDialog = false },
      title = { Text(text = label) },
      text = {
        OutlinedTextField(
          value = text,
          onValueChange = { text = it },
          singleLine = true,
          modifier = Modifier.fillMaxWidth(),
          keyboardOptions = keyboardOptions,
          keyboardActions = KeyboardActions(
            onDone = {
              if (isValid) {
                onConfirm(text)
                showDialog = false
              }
            }
          ),
          isError = !isValid,
          supportingText = {
            if (!isValid) {
              Text(errorMessage, color = MaterialTheme.colorScheme.error)
            }
          }
        )
      },
      confirmButton = {
        TextButton(
          onClick = {
            onConfirm(text)
            showDialog = false
          },
          enabled = isValid,
        ) {
          Text("Save")
        }
      },
      dismissButton = {
        TextButton(onClick = { showDialog = false }) {
          Text("Cancel")
        }
      }
    )
  }
}
