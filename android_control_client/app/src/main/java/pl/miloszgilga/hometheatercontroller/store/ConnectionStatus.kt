package pl.miloszgilga.hometheatercontroller.store

import androidx.compose.ui.graphics.Color
import pl.miloszgilga.hometheatercontroller.ui.theme.Error
import pl.miloszgilga.hometheatercontroller.ui.theme.Success
import pl.miloszgilga.hometheatercontroller.ui.theme.Warning

enum class ConnectionStatus(val color: Color) {
  DISCONNECTED(Error),
  CONNECTING(Warning),
  CONNECTED(Success),
  ERROR(Error),
  ;
}
