package pl.miloszgilga.htas.client.net

import androidx.annotation.StringRes
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.util.UiText

enum class DeviceError(
  val errorName: String,
  @param:StringRes val stringResId: Int,
) {
  INVALID_PASSWORD("APP_ERR_INVALID_PASSWORD", R.string.error_esp_invalid_password),
  INVALID_ARG("ESP_ERR_INVALID_ARG", R.string.error_esp_invalid_arg),
  NO_MEM("ESP_ERR_NO_MEM", R.string.error_esp_no_mem),
  INVALID_STATE("ESP_ERR_INVALID_STATE", R.string.error_esp_invalid_state),
  TIMEOUT("ESP_ERR_TIMEOUT", R.string.error_esp_timeout),
  ;

  companion object {
    fun fromErrorName(name: String) = entries.find { it.errorName == name }
  }
}

fun mapDeviceError(errorName: String, errorCode: String): UiText {
  val knownError = DeviceError.fromErrorName(errorName)
  return if (knownError != null) {
    UiText.StringResource(knownError.stringResId)
  } else {
    UiText.StringResource(R.string.error_esp_generic, errorName, errorCode)
  }
}
