package pl.miloszgilga.htas.client.toast

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun GlobalToastHost() {
  Box(
    modifier = Modifier
      .fillMaxSize()
      .padding(bottom = 100.dp, start = 16.dp, end = 16.dp),
    contentAlignment = Alignment.BottomCenter,
  ) {
    LazyColumn(
      verticalArrangement = Arrangement.spacedBy(8.dp),
      horizontalAlignment = Alignment.CenterHorizontally,
    ) {
      items(
        items = ToastManager.stack,
        key = { it.id },
      ) { toast ->
        ToastItemView(toast)
      }
    }
  }
}
