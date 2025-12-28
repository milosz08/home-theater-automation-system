package pl.miloszgilga.hometheatercontroller.ui

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.LockOpen
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.runtime.Composable
import androidx.navigation.NavController
import pl.miloszgilga.hometheatercontroller.store.AppStore
import pl.miloszgilga.hometheatercontroller.store.AppUiState

enum class AppScreen(
  val route: String,
  val title: String,
) {
  HOME("home", "Cinema Control") {
    @Composable
    override fun Actions(store: AppStore, state: AppUiState, navController: NavController) {
      IconButton(onClick = { store.toggleAppLock() }) {
        Icon(
          imageVector = if (state.isAppLocked) Icons.Filled.Lock else Icons.Filled.LockOpen,
          contentDescription = "Toggle Lock"
        )
      }
      IconButton(onClick = { navController.navigate(SETTINGS.route) }) {
        Icon(Icons.Filled.Settings, contentDescription = "Settings")
      }
    }
  },
  SETTINGS("settings", "Settings") {
    @Composable
    override fun NavigationIcon(navController: NavController) {
      IconButton(onClick = { navController.popBackStack() }) {
        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Return")
      }
    }
  },
  ;

  @Composable
  open fun NavigationIcon(navController: NavController) {
  }

  @Composable
  open fun Actions(store: AppStore, state: AppUiState, navController: NavController) {
  }

  companion object {
    fun fromRoute(route: String?): AppScreen = entries.find { it.route == route } ?: HOME
  }
}
