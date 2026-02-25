package pl.miloszgilga.htas.client

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.statusBars
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import pl.miloszgilga.htas.client.navigator.AppNavigator
import pl.miloszgilga.htas.client.toast.composable.GlobalToastHost
import pl.miloszgilga.htas.client.update.composable.FirmwareUpdateBanner
import pl.miloszgilga.htas.client.util.HTASClientTheme
import pl.miloszgilga.htas.client.viewmodel.MainViewModel

class MainActivity : ComponentActivity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    enableEdgeToEdge()
    setContent {
      HTASClientTheme {
        val viewModel: MainViewModel = viewModel(factory = MainViewModel.Factory)
        Column(
          modifier = Modifier
            .fillMaxSize()
            .windowInsetsPadding(WindowInsets.statusBars),
        ) {
          FirmwareUpdateBanner(viewModel = viewModel)
          Box(modifier = Modifier.weight(1f)) {
            AppNavigator(viewModel)
            GlobalToastHost()
          }
        }
      }
    }
  }
}
