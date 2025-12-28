package pl.miloszgilga.hometheatercontroller.ui

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.currentBackStackEntryAsState
import androidx.navigation.compose.rememberNavController
import pl.miloszgilga.hometheatercontroller.store.AppStore
import pl.miloszgilga.hometheatercontroller.ui.home.HomeScreen
import pl.miloszgilga.hometheatercontroller.ui.settings.SettingsScreen

@Composable
@OptIn(ExperimentalMaterial3Api::class)
fun AppNavigation(store: AppStore) {
  val navController = rememberNavController()
  val navBackStackEntry by navController.currentBackStackEntryAsState()
  val currentScreen = AppScreen.fromRoute(navBackStackEntry?.destination?.route)

  val state by store.uiState.collectAsState()

  Scaffold(
    modifier = Modifier.fillMaxSize(),
    topBar = {
      TopAppBar(
        title = { Text(currentScreen.title) },
        colors = TopAppBarDefaults.topAppBarColors(
          containerColor = MaterialTheme.colorScheme.primary,
          titleContentColor = MaterialTheme.colorScheme.onPrimary,
          navigationIconContentColor = MaterialTheme.colorScheme.onPrimary,
          actionIconContentColor = MaterialTheme.colorScheme.onPrimary
        ),
        navigationIcon = { currentScreen.NavigationIcon(navController) },
        actions = { currentScreen.Actions(store, state, navController) },
      )
    }
  ) {
    NavHost(
      navController = navController,
      startDestination = AppScreen.HOME.route,
      modifier = Modifier.padding(it)
    ) {
      composable(AppScreen.HOME.route) {
        HomeScreen(
          state = state,
          onRetry = store::checkConnection,
        )
      }
      composable(AppScreen.SETTINGS.route) {
        SettingsScreen(
          state = state,
          onConnectionSettingsChanged = store::updateConnectionSettings,
        )
      }
    }
  }
}
