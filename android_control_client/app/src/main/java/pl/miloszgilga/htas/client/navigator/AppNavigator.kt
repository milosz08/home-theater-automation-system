package pl.miloszgilga.htas.client.navigator

import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import pl.miloszgilga.htas.client.navigator.screen.MainScreen
import pl.miloszgilga.htas.client.viewmodel.MainViewModel
import pl.miloszgilga.htas.client.navigator.screen.SettingsScreen
import pl.miloszgilga.htas.client.navigator.screen.StatsScreen
import pl.miloszgilga.htas.client.viewmodel.AppUiState

@Composable
fun AppNavigator() {
  val navController = rememberNavController()
  val viewModel: MainViewModel = viewModel(factory = MainViewModel.Factory)
  val isConnected = viewModel.uiState is AppUiState.Connected

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
      LaunchedEffect(isConnected) {
        if (!isConnected) {
          navController.popBackStack(Screen.Home.route, inclusive = false)
        }
      }
      StatsScreen(
        viewModel = viewModel,
        onBack = { navController.popBackStack() },
      )
    }
  }
}
