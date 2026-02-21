package pl.miloszgilga.htas.client.net.ws

import com.google.gson.annotations.SerializedName

enum class WsAction(val key: String) {
  GET_SYS_INFO("cmd_get_sys_info"),

  // macros
  START_PROJECTION("cmd_start_projection"),
  END_PROJECTION("cmd_end_projection"),

  // projection screen
  SCREEN_DOWN("cmd_screen_down"),
  SCREEN_STOP("cmd_screen_stop"),
  SCREEN_UP("cmd_screen_up"),

  // projector
  PROJECTOR_ON("cmd_projector_on"),
  PROJECTOR_OFF("cmd_projector_off"),
  ;
}

data class WsRequest(
  @SerializedName("action") val action: String,
  @SerializedName("value") val value: Any? = null,
) {
  constructor(action: WsAction, value: Any? = null) : this(action.key, value)
}
