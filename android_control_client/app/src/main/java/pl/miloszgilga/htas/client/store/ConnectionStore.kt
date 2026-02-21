package pl.miloszgilga.htas.client.store

import android.content.Context
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.longPreferencesKey
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map

val Context.dataStore by preferencesDataStore(name = "settings")

class ConnectionStore(private val context: Context) {
  companion object {
    private val IP_KEY = stringPreferencesKey("ip")
    private val PORT_KEY = intPreferencesKey("port")
    private val PASS_KEY = stringPreferencesKey("pass")
    private val HASH_KEY = stringPreferencesKey("hash")
    private val LAST_CONNECTED_KEY = longPreferencesKey("last_connected")
  }

  val savedConfig: Flow<ServerConfig?> = context.dataStore.data.map { prefs ->
    val ip = prefs[IP_KEY]
    val port = prefs[PORT_KEY]
    val pass = prefs[PASS_KEY]
    val hash = prefs[HASH_KEY]
    if (ip != null && port != null && pass != null && hash != null) {
      ServerConfig(ip, port, pass, hash)
    } else {
      null
    }
  }

  val lastConnected: Flow<Long?> = context.dataStore.data.map {
    it[LAST_CONNECTED_KEY]
  }

  suspend fun saveConfig(config: ServerConfig) {
    context.dataStore.edit {
      it[IP_KEY] = config.ip
      it[PORT_KEY] = config.port
      it[PASS_KEY] = config.pass
      it[HASH_KEY] = config.hash
    }
  }

  suspend fun clearConfig() {
    context.dataStore.edit { it.clear() }
  }

  suspend fun saveLastConnected(timestamp: Long) {
    context.dataStore.edit { prefs ->
      prefs[LAST_CONNECTED_KEY] = timestamp
    }
  }
}
