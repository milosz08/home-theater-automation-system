package pl.miloszgilga.htas.client.navigator.screen

import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.runtime.remember
import androidx.compose.ui.res.stringResource
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.viewmodel.AppUiState
import pl.miloszgilga.htas.client.viewmodel.MainViewModel
import pl.miloszgilga.htas.client.util.calculateRam
import pl.miloszgilga.htas.client.composable.DataSection
import pl.miloszgilga.htas.client.composable.responsive.ResponsiveGrid
import pl.miloszgilga.htas.client.composable.ScrollableScreenWrapper
import pl.miloszgilga.htas.client.composable.SectionHeader
import pl.miloszgilga.htas.client.composable.SectionSpacer
import pl.miloszgilga.htas.client.composable.row.TextPropertyRow
import pl.miloszgilga.htas.client.composable.TopBarWrapper
import pl.miloszgilga.htas.client.composable.stats.StatsChart
import pl.miloszgilga.htas.client.util.appColors
import pl.miloszgilga.htas.client.util.formatUptime

@Composable
fun StatsScreen(
  viewModel: MainViewModel,
  onBack: () -> Unit,
) {
  val history = viewModel.envHistory
  val lastPoint = history.lastOrNull()

  val sysInfo = when (viewModel.uiState) {
    is AppUiState.Connected -> viewModel.sysInfo
    else -> null
  }

  TopBarWrapper(
    title = stringResource(R.string.device_statistics),
    onBack = onBack,
  ) {
    ScrollableScreenWrapper {
      if (sysInfo != null) {
        DataSection(stringResource(R.string.current_readings)) {
          TextPropertyRow(
            label = stringResource(R.string.uptime),
            value = if (lastPoint != null) {
              formatUptime(lastPoint.up)
            } else {
              "--"
            },
          )
        }
        SectionSpacer()

        SectionHeader(stringResource(R.string.historical_charts))
        ResponsiveGrid(
          items = listOf(
            {
              StatsChart(
                title = stringResource(R.string.temperature),
                dataPoints = history.map { it.temp.toFloat() },
                chartColor = MaterialTheme.appColors.temperature,
                leftSign = "°C",
              )
            },
            {
              StatsChart(
                title = stringResource(R.string.humidity),
                dataPoints = history.map { it.hum.toFloat() },
                chartColor = MaterialTheme.appColors.humidity,
                leftSign = "%",
              )
            },
            {
              StatsChart(
                title = stringResource(R.string.pressure),
                dataPoints = history.map { it.pres.toFloat() },
                chartColor = MaterialTheme.appColors.pressure,
                leftSign = "hPa",
              )
            },
            {
              val (currentUsedKb, totalRamKb, chartDataPoints) = remember(history.size, sysInfo) {
                calculateRam(sysInfo.ramMax, history.map { point -> point.ram })
              }
              StatsChart(
                title = stringResource(R.string.ram_usage),
                value = "$currentUsedKb / $totalRamKb kB",
                dataPoints = chartDataPoints,
                chartColor = MaterialTheme.appColors.ramUsage,
                leftSign = "kB",
              )
            }
          )
        )
        SectionSpacer()
      }
    }
  }
}
