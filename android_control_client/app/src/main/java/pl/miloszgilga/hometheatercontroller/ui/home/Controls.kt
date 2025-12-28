package pl.miloszgilga.hometheatercontroller.ui.home

import android.util.Log
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowDownward
import androidx.compose.material.icons.filled.ArrowUpward
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.Movie
import androidx.compose.material.icons.filled.Power
import androidx.compose.material.icons.filled.PowerOff
import androidx.compose.material.icons.filled.Stop
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import pl.miloszgilga.hometheatercontroller.store.AppUiState
import pl.miloszgilga.hometheatercontroller.ui.theme.BtnDanger

@Composable
fun Controls(state: AppUiState) {
  val areControlsEnabled = !state.isAppLocked
  Column(
    modifier = Modifier
      .fillMaxWidth()
      .padding(top = 24.dp, bottom = 24.dp),
    verticalArrangement = Arrangement.spacedBy(24.dp)
  ) {
    ControlSection("Projector + projection screen macros") {
      ControlBtn(
        text = "Start projection",
        icon = Icons.Filled.Movie,
        enabled = areControlsEnabled,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "MACRO START")
      }
      ControlBtn(
        text = "End projection",
        icon = Icons.Filled.Close,
        color = BtnDanger,
        enabled = areControlsEnabled,
        confirmationDialog = true,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "MACRO STOP")
      }
    }
    ControlSection(title = "Projection screen control section") {
      ControlBtn(
        text = "DOWN",
        icon = Icons.Filled.ArrowDownward,
        enabled = areControlsEnabled,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "SCREEN DOWN")
      }
      ControlBtn(
        text = "STOP",
        icon = Icons.Filled.Stop,
        color = BtnDanger,
        enabled = areControlsEnabled,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "SCREEN STOP")
      }
      ControlBtn(
        text = "UP",
        icon = Icons.Filled.ArrowUpward,
        enabled = areControlsEnabled,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "SCREEN UP")
      }
    }
    ControlSection(title = "Projector power control") {
      ControlBtn(
        text = "Turn ON",
        icon = Icons.Filled.Power,
        enabled = areControlsEnabled,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "PROJECTOR ON")
      }
      ControlBtn(
        text = "Turn OFF",
        icon = Icons.Filled.PowerOff,
        color = BtnDanger,
        enabled = areControlsEnabled,
        confirmationDialog = true,
      ) {
        // TODO
        Log.d("CC-REST-ACTION", "PROJECTOR OFF")
      }
    }
  }
}
