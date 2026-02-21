package pl.miloszgilga.htas.client.net.ws

import pl.miloszgilga.htas.client.net.JsonParser
import pl.miloszgilga.htas.client.net.parse

class WsEventParser(private val jsonParser: JsonParser) {
  fun parseEvent(jsonString: String): WsEvent {
    return try {
      val envelope = jsonParser.parse<WsRawEnvelope>(jsonString)
      when (envelope.type) {
        EnvelopeType.ENV -> jsonParser.fromJson(envelope.data.toString(), WsEvent.Env::class.java)

        EnvelopeType.SYS_INFO -> jsonParser.fromJson(envelope.data.toString(), WsEvent.SysInfo::class.java)

        EnvelopeType.CMD_INVOCATION -> jsonParser.fromJson(
          envelope.data.toString(),
          WsEvent.CommandInvocation::class.java
        )
      }
    } catch (ex: Exception) {
      WsEvent.ParseError(ex)
    }
  }
}
