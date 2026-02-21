package pl.miloszgilga.htas.client.net.ws

import com.google.gson.annotations.SerializedName

enum class WsAction(val key: String) {
  GET_SYS_INFO("cmd_get_sys_info"),
  ;
}

data class WsRequest(
  @SerializedName("action") val action: String,
  @SerializedName("value") val value: Any? = null,
)
