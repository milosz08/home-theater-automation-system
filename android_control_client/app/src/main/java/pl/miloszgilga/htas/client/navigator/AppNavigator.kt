package pl.miloszgilga.htas.client.navigator

import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import pl.miloszgilga.htas.client.navigator.screen.MainScreen
import pl.miloszgilga.htas.client.viewmodel.MainViewModel
import pl.miloszgilga.htas.client.navigator.screen.SettingsScreen
import pl.miloszgilga.htas.client.navigator.screen.StatsScreen
import pl.miloszgilga.htas.client.viewmodel.AppUiState

@Composable
fun AppNavigator(viewModel: MainViewModel) {
  val navController = rememberNavController()

  val isConnected = viewModel.uiState is AppUiState.Connected
  val isFirmwareUpdating = viewModel.uiState is AppUiState.FirmwareUpdating

  LaunchedEffect(viewModel.uiState) {
    if (isConnected || isFirmwareUpdating) {
      navController.popBackStack(Screen.Home.route, inclusive = false)
    }
  }

  NavHost(
    navController = navController,
    startDestination = Screen.Home.route
  ) {
    composable(route = Screen.Home.route) {
      MainScreen(
        viewModel = viewModel,
        onNavigateToSettings = {
          navController.navigate(Screen.Settings.route)
        },
        onNavigateToStats = {
          navController.navigate(Screen.Stats.route)
        },
      )
    }
    composable(route = Screen.Settings.route) {
      SettingsScreen(
        viewModel = viewModel,
        onForgetDevice = { navController.popBackStack(Screen.Home.route, inclusive = false) },
        onDisconnect = { navController.popBackStack(Screen.Home.route, inclusive = false) },
        onBack = { navController.popBackStack() },
      )
    }
    composable(route = Screen.Stats.route) {
      StatsScreen(
        viewModel = viewModel,
        onBack = { navController.popBackStack() },
      )
    }
  }
}
