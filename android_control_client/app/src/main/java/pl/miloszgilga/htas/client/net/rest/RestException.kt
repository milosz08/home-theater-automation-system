package pl.miloszgilga.htas.client.net.rest

import com.google.gson.annotations.SerializedName

data class ErrorPayload(
  @SerializedName("errorCode") val errorCode: String,
  @SerializedName("errorName") val errorName: String,
)

class RestException(
  httpCode: Int,
  val errorCode: String,
  val errorName: String
) : Exception("Rest error $httpCode: $errorName ($errorCode)")
