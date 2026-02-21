package pl.miloszgilga.htas.client.toast

import androidx.compose.runtime.mutableStateListOf
import java.util.UUID

enum class ToastType {
  NORMAL,
  ERROR,
  ;
}

data class ToastModel(
  val id: String = UUID.randomUUID().toString(),
  val message: String,
  val type: ToastType = ToastType.NORMAL,
  val duration: Long = 3000L,
)

object ToastManager {
  private val _stack = mutableStateListOf<ToastModel>()
  val stack: List<ToastModel> get() = _stack

  fun show(message: String, type: ToastType = ToastType.NORMAL) {
    val toast = ToastModel(message = message, type = type)
    _stack.add(toast)
  }

  fun dismiss(id: String) {
    _stack.removeIf { it.id == id }
  }
}
