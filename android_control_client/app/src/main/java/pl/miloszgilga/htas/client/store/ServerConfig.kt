package pl.miloszgilga.htas.client.store

data class ServerConfig(
  val ip: String,
  val port: Int,
  val pass: String,
  val hash: String,
)
