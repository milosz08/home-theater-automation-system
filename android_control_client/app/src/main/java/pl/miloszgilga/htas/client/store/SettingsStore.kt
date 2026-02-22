package pl.miloszgilga.htas.client.store

import android.content.Context
import androidx.datastore.preferences.core.booleanPreferencesKey
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.longPreferencesKey
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map

val Context.dataStore by preferencesDataStore(name = "settings")

class SettingsStore(private val context: Context) {
  companion object {
    const val INIT_COOLDOWN_DURATION_SEC = 5

    private val IP_KEY = stringPreferencesKey("ip")
    private val PORT_KEY = intPreferencesKey("port")
    private val PASS_KEY = stringPreferencesKey("pass")
    private val HASH_KEY = stringPreferencesKey("hash")
    private val LAST_CONNECTED_KEY = longPreferencesKey("last_connected")
    private val COOLDOWN_ENABLED_KEY = booleanPreferencesKey("cooldown_enabled")
    private val COOLDOWN_DURATION_KEY = intPreferencesKey("cooldown_duration")
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

  val cooldownEnabled: Flow<Boolean> = context.dataStore.data.map { prefs ->
    prefs[COOLDOWN_ENABLED_KEY] ?: true
  }

  val cooldownDurationSec: Flow<Int> = context.dataStore.data.map { prefs ->
    prefs[COOLDOWN_DURATION_KEY] ?: INIT_COOLDOWN_DURATION_SEC
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
    context.dataStore.edit { prefs ->
      prefs.remove(IP_KEY)
      prefs.remove(PORT_KEY)
      prefs.remove(PASS_KEY)
      prefs.remove(HASH_KEY)
    }
  }

  suspend fun saveLastConnected(timestamp: Long) {
    context.dataStore.edit { prefs ->
      prefs[LAST_CONNECTED_KEY] = timestamp
    }
  }

  suspend fun saveCooldownEnabled(enabled: Boolean) {
    context.dataStore.edit { prefs ->
      prefs[COOLDOWN_ENABLED_KEY] = enabled
    }
  }

  suspend fun saveCooldownDuration(seconds: Int) {
    context.dataStore.edit { prefs ->
      prefs[COOLDOWN_DURATION_KEY] = seconds
    }
  }
}
