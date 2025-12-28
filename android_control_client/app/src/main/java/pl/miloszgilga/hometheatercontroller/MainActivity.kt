package pl.miloszgilga.hometheatercontroller

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.lifecycle.viewmodel.compose.viewModel
import pl.miloszgilga.hometheatercontroller.store.AppStore
import pl.miloszgilga.hometheatercontroller.ui.AppNavigation
import pl.miloszgilga.hometheatercontroller.ui.theme.HomeTheaterControllerTheme

class MainActivity : ComponentActivity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    enableEdgeToEdge()
    setContent {
      val store: AppStore = viewModel(factory = AppStore.Factory)
      HomeTheaterControllerTheme {
        AppNavigation(store)
      }
    }
  }
}
