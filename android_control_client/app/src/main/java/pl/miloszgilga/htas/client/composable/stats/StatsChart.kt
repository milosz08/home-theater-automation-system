package pl.miloszgilga.htas.client.composable.stats

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import pl.miloszgilga.htas.client.composable.SectionSpacer
import pl.miloszgilga.htas.client.composable.SpacerSize

@Composable
fun StatsChart(
  title: String,
  dataPoints: List<Float>,
  chartColor: Color,
  leftSign: String,
  modifier: Modifier = Modifier,
  value: String? = null,
  noDataSign: String = "--",
) {
  Column(
    modifier = modifier
      .fillMaxWidth()
      .height(250.dp)
      .padding(16.dp),
  ) {
    Row(
      modifier = Modifier.fillMaxWidth(),
      horizontalArrangement = Arrangement.SpaceBetween,
    ) {
      Text(text = title, style = MaterialTheme.typography.titleSmall)
      Text(
        text = value ?: "${dataPoints.lastOrNull() ?: noDataSign} $leftSign",
        style = MaterialTheme.typography.titleMedium,
        color = chartColor,
      )
    }
    SectionSpacer(spacerSize = SpacerSize.SMALL)
    Box(modifier = Modifier.weight(1f)) {
      SimpleLineChart(
        modifier = Modifier.fillMaxSize(),
        dataPoints = dataPoints,
        lineColor = chartColor,
      )
    }
  }
}
