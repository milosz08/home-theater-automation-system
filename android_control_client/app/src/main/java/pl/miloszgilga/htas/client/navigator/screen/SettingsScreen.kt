package pl.miloszgilga.htas.client.navigator.screen

import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.BuildConfig
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.viewmodel.AppUiState
import pl.miloszgilga.htas.client.viewmodel.MainViewModel
import pl.miloszgilga.htas.client.composable.button.ButtonType
import pl.miloszgilga.htas.client.composable.ConfirmationAlert
import pl.miloszgilga.htas.client.composable.DataSection
import pl.miloszgilga.htas.client.composable.button.AppButton
import pl.miloszgilga.htas.client.composable.button.AppOutlinedButton
import pl.miloszgilga.htas.client.composable.ScrollableScreenWrapper
import pl.miloszgilga.htas.client.composable.SectionHeader
import pl.miloszgilga.htas.client.composable.SectionSpacer
import pl.miloszgilga.htas.client.composable.row.TextPropertyRow
import pl.miloszgilga.htas.client.composable.TopBarWrapper
import pl.miloszgilga.htas.client.composable.responsive.ResponsiveDualActionLayout
import pl.miloszgilga.htas.client.composable.settings.ChangePasswordModal
import pl.miloszgilga.htas.client.composable.settings.ConnectionStatusBadgeRow
import pl.miloszgilga.htas.client.composable.settings.FooterText
import pl.miloszgilga.htas.client.composable.settings.PasswordSettingsRow
import pl.miloszgilga.htas.client.util.formatTimestamp
import java.time.Instant
import java.time.ZoneId
import java.time.format.DateTimeFormatter

enum class ActiveDialog {
  NONE,
  DISCONNECT,
  FORGET,
  CHANGE_PASSWORD,
  RESET_PASSWORD,
  ;
}

@Composable
fun SettingsScreen(
  viewModel: MainViewModel,
  onDisconnect: () -> Unit,
  onForgetDevice: () -> Unit,
  onBack: () -> Unit,
) {
  val state = viewModel.uiState
  var activeDialog by remember { mutableStateOf(ActiveDialog.NONE) }

  val config = when (state) {
    is AppUiState.Connected -> state.config
    is AppUiState.ConnectionFailed -> state.config
    is AppUiState.Disconnected -> state.config
    else -> null
  }
  val sysInfo = when (state) {
    is AppUiState.Connected -> viewModel.sysInfo
    else -> null
  }
  val buildDate = remember {
    val formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm").withZone(ZoneId.systemDefault())
    formatter.format(Instant.ofEpochMilli(BuildConfig.BUILD_TIME))
  }

  TopBarWrapper(
    title = stringResource(R.string.settings),
    onBack = onBack,
  ) {
    ScrollableScreenWrapper {
      DataSection(stringResource(R.string.base_info)) {
        ConnectionStatusBadgeRow(state is AppUiState.Connected)
      }
      SectionSpacer()

      config?.let {
        DataSection(stringResource(R.string.connection_details)) {
          TextPropertyRow(label = stringResource(R.string.ip_address), value = it.ip)
          TextPropertyRow(label = stringResource(R.string.port), value = it.port.toString())
          PasswordSettingsRow(label = stringResource(R.string.password), password = it.pass)
          sysInfo?.let {
            ResponsiveDualActionLayout(
              primaryAction = { weightModifier ->
                AppButton(
                  text = stringResource(R.string.change_password),
                  onClick = { activeDialog = ActiveDialog.CHANGE_PASSWORD },
                  modifier = weightModifier
                )
              },
              secondaryAction = { weightModifier ->
                AppButton(
                  text = stringResource(R.string.reset_password),
                  onClick = { activeDialog = ActiveDialog.RESET_PASSWORD },
                  modifier = weightModifier,
                  type = ButtonType.ERROR,
                )
              }
            )
          }
          TextPropertyRow(
            label = stringResource(R.string.last_connection),
            value = formatTimestamp(viewModel.lastConnectedTimestamp),
          )
        }
      }
      SectionSpacer()

      sysInfo?.let {
        DataSection(stringResource(R.string.device_firmware)) {
          TextPropertyRow(label = stringResource(R.string.firmware_name), value = it.project)
          TextPropertyRow(label = stringResource(R.string.firmware_version), value = it.version)
          TextPropertyRow(label = stringResource(R.string.compiled_at), value = "${it.compileDate} ${it.compileTime}")
        }
      }
      SectionSpacer()

      SectionHeader(
        text = stringResource(R.string.danger_zone),
        color = MaterialTheme.colorScheme.error,
      )
      ResponsiveDualActionLayout(
        modifier = Modifier.padding(horizontal = 16.dp),
        primaryAction = { weightModifier ->
          AppButton(
            text = stringResource(R.string.disconnect),
            onClick = { activeDialog = ActiveDialog.DISCONNECT },
            enabled = state is AppUiState.Connected,
            modifier = weightModifier,
            type = ButtonType.ERROR,
          )
        },
        secondaryAction = { weightModifier ->
          AppOutlinedButton(
            text = stringResource(R.string.forget_unpair),
            onClick = { activeDialog = ActiveDialog.FORGET },
            modifier = weightModifier,
            type = ButtonType.ERROR,
          )
        }
      )
      SectionSpacer()

      FooterText(label = stringResource(R.string.app_version), value = BuildConfig.CI_BUILD_VERSION)
      FooterText(label = stringResource(R.string.build_time), value = buildDate)
      SectionSpacer()

      when (activeDialog) {
        ActiveDialog.CHANGE_PASSWORD -> {
          ChangePasswordModal(
            onDismiss = { activeDialog = ActiveDialog.NONE },
            onConfirm = { oldPass, newPass ->
              viewModel.changeDevicePassword(oldPass, newPass)
              activeDialog = ActiveDialog.NONE
            }
          )
        }

        ActiveDialog.RESET_PASSWORD -> {
          ConfirmationAlert(
            title = stringResource(R.string.reset_password_alert),
            text = stringResource(R.string.reset_password_alert_description),
            confirmText = stringResource(R.string.reset_password),
            isSolidConfirmButton = true,
            onConfirm = {
              viewModel.resetPassword()
              activeDialog = ActiveDialog.NONE
            },
            onDismiss = { activeDialog = ActiveDialog.NONE },
          )
        }

        ActiveDialog.DISCONNECT -> {
          ConfirmationAlert(
            title = stringResource(R.string.disconnect_alert),
            text = stringResource(R.string.disconnect_alert_description),
            confirmText = stringResource(R.string.disconnect),
            isSolidConfirmButton = true,
            onConfirm = {
              activeDialog = ActiveDialog.NONE
              config?.let { viewModel.manuallyDisconnect(it) }
              onDisconnect()
            },
            onDismiss = { activeDialog = ActiveDialog.NONE },
          )
        }

        ActiveDialog.FORGET -> {
          ConfirmationAlert(
            title = stringResource(R.string.forget_unpair_alert),
            text = stringResource(R.string.forget_unpair_alert_description),
            confirmText = stringResource(R.string.forget_unpair),
            onConfirm = {
              activeDialog = ActiveDialog.NONE
              viewModel.forgetDevice()
              onForgetDevice()
            },
            onDismiss = { activeDialog = ActiveDialog.NONE },
          )
        }

        ActiveDialog.NONE -> {
        }
      }
    }
  }
}
