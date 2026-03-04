package pl.miloszgilga.htas.client.update.composable

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.expandVertically
import androidx.compose.animation.shrinkVertically
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Clear
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.ConfirmationAlert
import pl.miloszgilga.htas.client.viewmodel.AppUiState
import pl.miloszgilga.htas.client.viewmodel.MainViewModel

@Composable
fun FirmwareUpdateBanner(
  viewModel: MainViewModel,
  modifier: Modifier = Modifier,
) {
  var showConfirmModal by remember { mutableStateOf(false) }
  val latestVersion by viewModel.latestVersion.collectAsState()

  AnimatedVisibility(
    visible = latestVersion != null && viewModel.uiState is AppUiState.Connected,
    enter = expandVertically(),
    exit = shrinkVertically(),
    modifier = modifier,
  ) {
    Surface(
      color = MaterialTheme.colorScheme.tertiaryContainer,
      modifier = Modifier.fillMaxWidth(),
    ) {
      Row(
        modifier = Modifier
          .fillMaxWidth()
          .padding(start = 16.dp, end = 4.dp, top = 4.dp, bottom = 4.dp),
        verticalAlignment = Alignment.CenterVertically,
      ) {
        Row(
          modifier = Modifier.weight(1f),
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.SpaceBetween,
        ) {
          Text(
            text = stringResource(R.string.update_banner_message, latestVersion ?: "---"),
            style = MaterialTheme.typography.bodyMedium,
            color = MaterialTheme.colorScheme.onTertiaryContainer,
            modifier = Modifier.weight(1f),
          )
          TextButton(onClick = { showConfirmModal = true }) {
            Text(stringResource(R.string.update_banner_button))
          }
        }
        IconButton(
          onClick = { viewModel.hideUpdateBanner() },
        ) {
          Icon(
            imageVector = Icons.Default.Clear,
            contentDescription = "Dismiss",
            tint = MaterialTheme.colorScheme.onTertiaryContainer,
            modifier = Modifier.size(20.dp),
          )
        }
      }
    }
  }
  if (showConfirmModal) {
    ConfirmationAlert(
      title = stringResource(R.string.update_dialog_title),
      text = stringResource(R.string.update_dialog_message, latestVersion ?: "---"),
      confirmText = stringResource(R.string.update_dialog_confirm),
      onConfirm = {
        viewModel.startFirmwareUpdate()
        showConfirmModal = false
      },
      onDismiss = {
        viewModel.hideUpdateBanner()
        showConfirmModal = false
      },
      isSolidConfirmButton = true,
    )
  }
}
