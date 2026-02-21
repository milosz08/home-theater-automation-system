package pl.miloszgilga.htas.client

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.ui.Modifier
import pl.miloszgilga.htas.client.navigator.AppNavigator
import pl.miloszgilga.htas.client.toast.GlobalToastHost
import pl.miloszgilga.htas.client.util.HTASClientTheme

class MainActivity : ComponentActivity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    enableEdgeToEdge()
    setContent {
      HTASClientTheme {
        Box(modifier = Modifier.fillMaxSize()) {
          AppNavigator()
          GlobalToastHost()
        }
      }
    }
  }
}
