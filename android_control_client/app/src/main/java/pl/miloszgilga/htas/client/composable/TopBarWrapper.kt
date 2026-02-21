package pl.miloszgilga.htas.client.composable

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ColumnScope
import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.R

enum class TopBarType {
  ROOT_PAGE,
  SUB_PAGE,
  ;
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TopBarWrapper(
  title: String,
  modifier: Modifier = Modifier,
  onBack: () -> Unit = {},
  topBarType: TopBarType = TopBarType.SUB_PAGE,
  mainNavigationContent: @Composable RowScope.() -> Unit = {},
  content: @Composable ColumnScope.() -> Unit,
) {
  Scaffold(
    topBar = {
      TopAppBar(
        title = { Text(title) },
        navigationIcon = {
          if (topBarType == TopBarType.SUB_PAGE) {
            IconButton(onClick = onBack) {
              Icon(
                imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                contentDescription = stringResource(R.string.back),
              )
            }
          }
        },
        actions = {
          if (topBarType == TopBarType.ROOT_PAGE) {
            mainNavigationContent()
          }
        },
      )
    }
  ) { innerPadding ->
    Column(
      modifier = modifier
        .padding(innerPadding)
        .fillMaxSize()
        .padding(20.dp),
      horizontalAlignment = Alignment.CenterHorizontally,
      content = content,
    )
  }
}
