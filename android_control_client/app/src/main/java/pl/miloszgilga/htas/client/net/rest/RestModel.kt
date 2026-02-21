package pl.miloszgilga.htas.client.net.rest

import com.google.gson.annotations.SerializedName

data class ResetPasswordResponse(
  @SerializedName("defaultPassword") val defaultPassword: String,
)
