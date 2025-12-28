package pl.miloszgilga.hometheatercontroller.store

import android.app.Application
import android.content.Context
import androidx.datastore.preferences.core.booleanPreferencesKey
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.intPreferencesKey
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProvider.AndroidViewModelFactory.Companion.APPLICATION_KEY
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.initializer
import androidx.lifecycle.viewmodel.viewModelFactory
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.distinctUntilChanged
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import pl.miloszgilga.hometheatercontroller.net.RestServerConnectivityService
import pl.miloszgilga.hometheatercontroller.net.ServerConnectivityService

private val Context.dataStore by preferencesDataStore(name = "app_settings")

class AppStore(
  private val serverConnectivityService: ServerConnectivityService,
  application: Application,
) : AndroidViewModel(application) {
  companion object {
    val Factory: ViewModelProvider.Factory = viewModelFactory {
      val connectivityService = RestServerConnectivityService()
      initializer { AppStore(connectivityService, (this[APPLICATION_KEY] as Application)) }
    }
    private const val HEARTBEAT_INTERVAL_SEC = 5;
  }

  private val ipKey = stringPreferencesKey("ip_address")
  private val restApiPortKey = intPreferencesKey("rest_api_port")
  private val appLockedKey = booleanPreferencesKey("is_app_locked")

  private val _context = getApplication<Application>() // for file-based persistence
  private val _uiState = MutableStateFlow(AppUiState()) // for memory-based superfast persistence
  private var heartbeatJob: Job? = null // for checking network connection

  val uiState = _uiState.asStateFlow()

  // load settings directly on application startup
  init {
    viewModelScope.launch {
      _context.dataStore.data.collect { preferences ->
        _uiState.update { currentState ->
          currentState.copy(
            ipAddress = preferences[ipKey] ?: _uiState.value.ipAddress,
            restApiPort = (preferences[restApiPortKey] ?: _uiState.value.restApiPort).toString(),
            isAppLocked = preferences[appLockedKey] ?: _uiState.value.isAppLocked,
          )
        }
      }
    }
    // triggered ONLY when IP or port changes (prevent checking connection on change rest of state).
    viewModelScope.launch {
      _context.dataStore.data
        .map { prefs -> Pair(prefs[ipKey], prefs[restApiPortKey]) }
        .distinctUntilChanged()
        .collect { restartConnectionMonitoring() }
    }
  }

  fun toggleAppLock() {
    val newState = !_uiState.value.isAppLocked
    _uiState.update { it.copy(isAppLocked = newState) }
    viewModelScope.launch {
      getApplication<Application>().dataStore.edit { it[appLockedKey] = newState }
    }
  }

  fun updateConnectionSettings(newIp: String, newPort: String) {
    _uiState.update {
      it.copy(
        ipAddress = newIp,
        restApiPort = newPort,
        connectionStatus = ConnectionStatus.DISCONNECTED,
      )
    }
    viewModelScope.launch {
      _context.dataStore.edit {
        it[ipKey] = newIp
        it[restApiPortKey] = newPort.toInt() // checked before
      }
    }
  }

  fun checkConnection() {
    restartConnectionMonitoring()
  }

  private fun restartConnectionMonitoring() {
    heartbeatJob?.cancel()
    _uiState.update { it.copy(connectionStatus = ConnectionStatus.CONNECTING) }
    heartbeatJob = viewModelScope.launch {
      performCheck()
      while (isActive) {
        delay(HEARTBEAT_INTERVAL_SEC * 1000L)
        performCheck()
      }
    }
  }

  private suspend fun performCheck() {
    val isReachable = serverConnectivityService.isReachable(
      ip = _uiState.value.ipAddress,
      port = _uiState.value.restApiPort.toInt(),
    )
    _uiState.update {
      it.copy(connectionStatus = if (isReachable) ConnectionStatus.CONNECTED else ConnectionStatus.ERROR)
    }
  }
}
