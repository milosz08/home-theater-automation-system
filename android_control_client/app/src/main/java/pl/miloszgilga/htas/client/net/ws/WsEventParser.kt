package pl.miloszgilga.htas.client.net.ws

import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.parse

class WsEventParser(private val jsonParser: JsonParser) {
  fun parseEvent(jsonString: String): WsEvent {
    return try {
      val envelope = jsonParser.parse<WsRawEnvelope>(jsonString)
      when (envelope.type) {
        "env" -> jsonParser.fromJson(envelope.data.toString(), WsEvent.Env::class.java)
        "sys-info" -> jsonParser.fromJson(envelope.data.toString(), WsEvent.SysInfo::class.java)
        else -> WsEvent.Unknown(envelope.type)
      }
    } catch (ex: Exception) {
      WsEvent.ParseError(ex)
    }
  }
}
