package pl.miloszgilga.htas.client.util

import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

fun formatTimestamp(timestamp: Long?): String {
  if (timestamp == null) {
    return "--"
  }
  val sdf = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault())
  return sdf.format(Date(timestamp))
}

fun formatUptime(seconds: Long): String {
  if (seconds < 60) {
    return "${seconds}s"
  }
  val days = seconds / (24 * 3600)
  val hours = (seconds % (24 * 3600)) / 3600
  val minutes = (seconds % 3600) / 60
  return if (days > 0) {
    "${days}d ${hours}h ${minutes}m"
  } else if (hours > 0) {
    "${hours}h ${minutes}m"
  } else {
    "${minutes}m"
  }
}

fun calculateRam(
  totalRamBytes: Long,
  freeRamHistoryBytes: List<Long>,
): Triple<Int, Int, List<Float>> {
  val totalRamKb = totalRamBytes / 1024f
  val historyUsedKb = freeRamHistoryBytes.map { freeRam ->
    (totalRamBytes - freeRam) / 1024f
  }
  val currentFree = freeRamHistoryBytes.lastOrNull() ?: 0L
  val currentUsed = if (freeRamHistoryBytes.isNotEmpty()) {
    totalRamBytes - currentFree
  } else {
    0L
  }
  val currentUsedKb = currentUsed / 1024f
  return Triple(currentUsedKb.toInt(), totalRamKb.toInt(), historyUsedKb)
}
