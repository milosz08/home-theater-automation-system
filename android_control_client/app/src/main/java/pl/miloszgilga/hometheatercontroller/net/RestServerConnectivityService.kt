package pl.miloszgilga.hometheatercontroller.net

import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

class RestServerConnectivityService : ServerConnectivityService {
  override suspend fun isReachable(ip: String, port: Int): Boolean {
    return withContext(Dispatchers.IO) {
      try {
        Log.d("CC-HEARTBEAT", "Pinging: ${ip}:${port}")
        // TODO: check server connection, true if connection is up, otherwise false
        true
      } catch (e: Exception) {
        Log.d("CC-HEARTBEAT", "Error: ${e.message}")
        false
      }
    }
  }
}
