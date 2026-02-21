package pl.miloszgilga.htas.client.navigator

sealed class Screen(val route: String) {
  object Home : Screen("home")
  object Settings : Screen("settings")
  object Stats : Screen("stats")
}
