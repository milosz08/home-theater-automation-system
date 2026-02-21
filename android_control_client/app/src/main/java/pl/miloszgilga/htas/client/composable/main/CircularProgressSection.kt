package pl.miloszgilga.htas.client.composable.main

import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import pl.miloszgilga.htas.client.composable.CenteredScreenWrapper
import pl.miloszgilga.htas.client.composable.SectionSpacer

@Composable
fun CircularProgressSection(loadingText: String) {
  CenteredScreenWrapper {
    CircularProgressIndicator()
    SectionSpacer()
    Text("$loadingText...", style = MaterialTheme.typography.bodyLarge)
  }
}
