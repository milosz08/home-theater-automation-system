package pl.miloszgilga.hometheatercontroller.net

interface ServerConnectivityService {
  suspend fun isReachable(ip: String, port: Int): Boolean
}
