package pl.miloszgilga.htas.client.net

import com.google.gson.Gson
import com.google.gson.JsonSyntaxException

sealed class JsonParseException(message: String, cause: Throwable? = null) : Exception(message, cause) {
  class EmptyResult : JsonParseException("parsed object is null")
  class InvalidFormat(cause: Throwable) : JsonParseException("invalid JSON syntax", cause)
  class Unknown(cause: Throwable) : JsonParseException("unknown parsing error", cause)
}

interface JsonParser {
  @Throws(Exception::class)
  fun <T> fromJson(json: String, classOfT: Class<T>): T

  fun <T> toJson(obj: T): String
}

class GsonJsonParser(private val gson: Gson = Gson()) : JsonParser {
  override fun <T> fromJson(json: String, classOfT: Class<T>): T {
    try {
      return gson.fromJson(json, classOfT)
        ?: throw JsonParseException.EmptyResult()
    } catch (ex: JsonSyntaxException) {
      throw JsonParseException.InvalidFormat(ex)
    } catch (ex: Exception) {
      throw JsonParseException.Unknown(ex)
    }
  }

  override fun <T> toJson(obj: T): String {
    return gson.toJson(obj)
  }
}

inline fun <reified T> JsonParser.parse(json: String): T {
  return this.fromJson(json, T::class.java)
}
