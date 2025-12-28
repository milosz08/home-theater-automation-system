package pl.miloszgilga.hometheatercontroller.ui.home

import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun RowScope.ControlBtn(
  text: String,
  icon: ImageVector? = null,
  modifier: Modifier = Modifier,
  color: Color = MaterialTheme.colorScheme.primary,
  enabled: Boolean = true,
  confirmationDialog: Boolean = false,
  onClick: () -> Unit,
) {
  var showDialog by remember { mutableStateOf(false) }

  Button(
    enabled = enabled,
    onClick = {
      if (confirmationDialog) {
        showDialog = true
      } else {
        onClick()
      }
    },
    modifier = modifier
      .weight(1f)
      .height(64.dp),
    shape = MaterialTheme.shapes.medium,
    colors = ButtonDefaults.buttonColors(
      containerColor = color,
      contentColor = MaterialTheme.colorScheme.onPrimary,
    )
  ) {
    icon?.let {
      Icon(
        imageVector = it,
        contentDescription = null,
        modifier = Modifier.size(24.dp),
      )
      Spacer(modifier = Modifier.width(8.dp))
    }
    Text(
      text = text,
      fontSize = 16.sp,
      fontWeight = FontWeight.SemiBold,
    )
  }
  if (showDialog && confirmationDialog) {
    AlertDialog(
      onDismissRequest = { showDialog = false },
      title = { Text(text) },
      text = { Text(text = "Confirm action: \"$text\"!") },
      containerColor = MaterialTheme.colorScheme.surface,
      confirmButton = {
        TextButton(
          onClick = {
            showDialog = false
            onClick()
          }
        ) {
          Text("Confirm", color = color, fontWeight = FontWeight.Bold)
        }
      },
      dismissButton = {
        TextButton(onClick = { showDialog = false }) {
          Text("Cancel", color = MaterialTheme.colorScheme.onSurface)
        }
      }
    )
  }
}
