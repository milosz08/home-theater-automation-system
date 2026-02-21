package pl.miloszgilga.htas.client.net.ws

import com.google.gson.JsonElement
import com.google.gson.annotations.SerializedName

sealed class WsEvent {
  data class Env(
    @SerializedName("temp") val temp: Double,
    @SerializedName("hum") val hum: Int,
    @SerializedName("pres") val pres: Double,
    @SerializedName("ram") val ram: Long,
    @SerializedName("up") val up: Long,
  ) : WsEvent()

  data class SysInfo(
    @SerializedName("version") val version: String,
    @SerializedName("project") val project: String,
    @SerializedName("compileDate") val compileDate: String,
    @SerializedName("compileTime") val compileTime: String,
    @SerializedName("ramMax") val ramMax: Long,
  ) : WsEvent()

  data class CommandInvocation(
    @SerializedName("cmd") val cmd: String,
    @SerializedName("errorCode") val errorCode: String?,
    @SerializedName("errorName") val errorName: String?,
  ) : WsEvent()

  data class Unknown(val type: String) : WsEvent()

  data class ParseError(val cause: Throwable) : WsEvent()
}

enum class EnvelopeType {
  @SerializedName("env")
  ENV,

  @SerializedName("sys-info")
  SYS_INFO,

  @SerializedName("cmd-invocation")
  CMD_INVOCATION,
  ;
}

data class WsRawEnvelope(
  @SerializedName("type") val type: EnvelopeType,
  @SerializedName("data") val data: JsonElement,
)
