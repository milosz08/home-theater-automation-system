package pl.miloszgilga.htas.client.composable.main

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDownward
import androidx.compose.material.icons.filled.ArrowUpward
import androidx.compose.material.icons.filled.Stop
import androidx.compose.runtime.Composable
import androidx.compose.ui.res.stringResource
import pl.miloszgilga.htas.client.R
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
  ScrollableScreenWrapper {
    SectionHeader(stringResource(R.string.macros))
    ResponsiveMultipleActionLayout(
      actions = listOf(
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.start_projection),
            onClick = { viewModel.sendCommand(WsAction.START_PROJECTION) },
            size = ButtonSize.LARGE,
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.end_projection),
            onClick = { viewModel.sendCommand(WsAction.END_PROJECTION) },
            type = ButtonType.ERROR,
            size = ButtonSize.LARGE,
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
            onClick = { viewModel.sendCommand(WsAction.SCREEN_UP) },
            size = ButtonSize.LARGE,
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.screen_stop),
            icon = Icons.Default.Stop,
            onClick = { viewModel.sendCommand(WsAction.SCREEN_STOP) },
            type = ButtonType.ERROR,
            size = ButtonSize.LARGE,
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.screen_down),
            icon = Icons.Default.ArrowDownward,
            onClick = { viewModel.sendCommand(WsAction.SCREEN_DOWN) },
            size = ButtonSize.LARGE,
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
            onClick = { viewModel.sendCommand(WsAction.PROJECTOR_ON) },
            size = ButtonSize.LARGE,
            modifier = weightModifier,
          )
        },
        { weightModifier ->
          AppButton(
            text = stringResource(R.string.projector_off),
            onClick = { viewModel.sendCommand(WsAction.PROJECTOR_OFF) },
            type = ButtonType.ERROR,
            size = ButtonSize.LARGE,
            modifier = weightModifier,
          )
        },
      ),
    )
  }
}
