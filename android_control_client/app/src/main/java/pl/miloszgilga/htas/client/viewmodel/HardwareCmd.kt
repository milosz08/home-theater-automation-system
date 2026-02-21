package pl.miloszgilga.htas.client.viewmodel

import androidx.annotation.StringRes
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.net.ws.WsAction

enum class HardwareCmd(
  val wsAction: WsAction,
  @param:StringRes val resId: Int,
) {
  // macros
  START_PROJECTION(WsAction.START_PROJECTION, R.string.cmd_start_projection),
  END_PROJECTION(WsAction.END_PROJECTION, R.string.cmd_end_projection),

  // projection screen
  SCREEN_DOWN(WsAction.SCREEN_DOWN, R.string.cmd_screen_down),
  SCREEN_STOP(WsAction.SCREEN_STOP, R.string.cmd_screen_stop),
  SCREEN_UP(WsAction.SCREEN_UP, R.string.cmd_screen_up),

  // projector
  PROJECTOR_ON(WsAction.PROJECTOR_ON, R.string.cmd_projector_on),
  PROJECTOR_OFF(WsAction.PROJECTOR_OFF, R.string.cmd_projector_off),
  ;

  companion object {
    fun fromKey(key: String): HardwareCmd? = entries.find { it.wsAction.key == key }
  }
}
