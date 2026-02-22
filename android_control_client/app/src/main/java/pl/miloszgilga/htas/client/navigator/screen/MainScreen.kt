package pl.miloszgilga.htas.client.navigator.screen

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.BarChart
import androidx.compose.material.icons.filled.CloudOff
import androidx.compose.material.icons.filled.DeviceUnknown
import androidx.compose.material.icons.filled.LinkOff
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.LockOpen
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.filled.Warning
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.res.stringResource
import pl.miloszgilga.htas.client.composable.button.TooltipIconButton
import pl.miloszgilga.htas.client.viewmodel.AppUiState
import pl.miloszgilga.htas.client.viewmodel.MainViewModel
import pl.miloszgilga.htas.client.composable.TopBarType
import pl.miloszgilga.htas.client.composable.TopBarWrapper
import pl.miloszgilga.htas.client.qr.QrScanModal
import pl.miloszgilga.htas.client.toast.ToastManager
import pl.miloszgilga.htas.client.toast.ToastType
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.main.CircularProgressSection
import pl.miloszgilga.htas.client.composable.main.ConnectedSection
import pl.miloszgilga.htas.client.composable.main.StateMessageType
import pl.miloszgilga.htas.client.composable.main.UnifiedStateSection

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
  viewModel: MainViewModel,
  onNavigateToSettings: () -> Unit,
  onNavigateToStats: () -> Unit,
) {
  val state = viewModel.uiState
  var showQrModal by remember { mutableStateOf(false) }

  TopBarWrapper(
    title = stringResource(R.string.app_name),
    topBarType = TopBarType.ROOT_PAGE,
    mainNavigationContent = {
      TooltipIconButton(
        icon = if (viewModel.isUiLocked) Icons.Default.Lock else Icons.Default.LockOpen,
        contentDescription = stringResource(if (viewModel.isUiLocked) R.string.unblock_ui else R.string.block_ui),
        onClick = { viewModel.toggleUiLock() },
        enabled = state is AppUiState.Connected,
        disabledMessage = stringResource(R.string.unblock_block_ui_tooltip),
      )
      TooltipIconButton(
        icon = Icons.Default.BarChart,
        contentDescription = stringResource(R.string.device_statistics),
        onClick = onNavigateToStats,
        enabled = state is AppUiState.Connected,
        disabledMessage = stringResource(R.string.device_statistics_tooltip),
      )
      TooltipIconButton(
        icon = Icons.Default.Settings,
        contentDescription = stringResource(R.string.settings),
        onClick = onNavigateToSettings,
        enabled = state !is AppUiState.Unpaired,
        disabledMessage = stringResource(R.string.settings_tooltip),
      )
    },
  ) {
    when (state) {
      is AppUiState.Loading -> CircularProgressSection(stringResource(R.string.loading))

      is AppUiState.Connecting -> CircularProgressSection(stringResource(R.string.connecting))

      is AppUiState.Connected -> ConnectedSection(viewModel)

      is AppUiState.Unpaired -> UnifiedStateSection(
        icon = Icons.Default.DeviceUnknown,
        title = stringResource(R.string.no_device_paired),
        description = stringResource(R.string.no_device_paired_description),
        primaryButtonText = stringResource(R.string.open_qr_scanner),
        onPrimaryClick = { showQrModal = true },
        type = StateMessageType.NORMAL,
      )

      is AppUiState.Disconnected -> UnifiedStateSection(
        icon = Icons.Default.LinkOff,
        title = stringResource(R.string.connection_lost),
        description = stringResource(R.string.connection_lost_description),
        primaryButtonText = stringResource(R.string.reconnect),
        onPrimaryClick = { viewModel.retryConnection(state.config) },
        secondaryButtonText = stringResource(R.string.scan_other_device),
        onSecondaryClick = { showQrModal = true },
        type = StateMessageType.NORMAL,
      )

      is AppUiState.QrScanError -> UnifiedStateSection(
        icon = Icons.Default.Warning,
        title = stringResource(R.string.qr_scan_error),
        description = state.error.asString(),
        primaryButtonText = stringResource(R.string.scan_again),
        onPrimaryClick = { showQrModal = true },
        type = StateMessageType.ERROR,
      )

      is AppUiState.ConnectionFailed -> UnifiedStateSection(
        icon = Icons.Default.CloudOff,
        title = stringResource(R.string.connection_failed),
        description = state.error.asString(),
        primaryButtonText = stringResource(R.string.try_again),
        onPrimaryClick = { viewModel.retryConnection(state.config) },
        type = StateMessageType.ERROR,
      )
    }
    if (showQrModal) {
      val devicePairedMsg = stringResource(R.string.device_paired)
      QrScanModal(
        viewModel = viewModel,
        onDismiss = { showQrModal = false },
        onCodeScanned = { json ->
          showQrModal = false
          if (viewModel.onQrScanned(json)) {
            ToastManager.show(devicePairedMsg, ToastType.NORMAL)
          }
        },
      )
    }
  }
}
