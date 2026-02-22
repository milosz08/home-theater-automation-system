package pl.miloszgilga.htas.client.composable.main

import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDownward
import androidx.compose.material.icons.filled.ArrowUpward
import androidx.compose.material.icons.filled.Stop
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.AnimatedProgressBar
import pl.miloszgilga.htas.client.composable.ScrollableScreenWrapper
import pl.miloszgilga.htas.client.composable.SectionHeader
import pl.miloszgilga.htas.client.composable.button.AppButton
import pl.miloszgilga.htas.client.composable.button.ButtonSize
import pl.miloszgilga.htas.client.composable.button.ButtonType
import pl.miloszgilga.htas.client.composable.responsive.ResponsiveMultipleActionLayout
import pl.miloszgilga.htas.client.net.ws.WsAction
import pl.miloszgilga.htas.client.viewmodel.MainViewModel

@Composable
fun ConnectedSection(viewModel: MainViewModel) {
  val isLocked = viewModel.isUiLocked

  ScrollableScreenWrapper {
    AnimatedProgressBar(
      isVisible = isLocked && viewModel.cooldownProgress > 0f,
      progress = viewModel.cooldownProgress,
      modifier = Modifier.padding(bottom = 8.dp)
    )

    SectionHeader(stringResource(R.string.macros))
    ResponsiveMultipleActionLayout(
      actions = listOf(
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.start_projection),
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.START_PROJECTION) },
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.end_projection),
            type = ButtonType.ERROR,
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.END_PROJECTION) },
            modifier = weightModifier,
          )
        },
      ),
    )
    SectionHeader(stringResource(R.string.projection_screen_control))
    ResponsiveMultipleActionLayout(
      actions = listOf(
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.screen_up),
            icon = Icons.Default.ArrowUpward,
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.SCREEN_UP) },
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.screen_stop),
            icon = Icons.Default.Stop,
            type = ButtonType.ERROR,
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.SCREEN_STOP) },
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.screen_down),
            icon = Icons.Default.ArrowDownward,
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.SCREEN_DOWN) },
            modifier = weightModifier,
          )
        },
      ),
    )
    SectionHeader(stringResource(R.string.projector_control))
    ResponsiveMultipleActionLayout(
      actions = listOf(
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.projector_on),
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.PROJECTOR_ON) },
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.projector_off),
            type = ButtonType.ERROR,
            size = ButtonSize.LARGE,
            enabled = !isLocked,
            onClick = { viewModel.sendCommand(WsAction.PROJECTOR_OFF) },
            modifier = weightModifier,
          )
        },
      ),
    )
  }
}
