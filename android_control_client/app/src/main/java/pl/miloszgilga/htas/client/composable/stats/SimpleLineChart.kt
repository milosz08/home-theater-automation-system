package pl.miloszgilga.htas.client.composable.stats

import android.view.ViewGroup
import android.widget.LinearLayout
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.viewinterop.AndroidView
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import pl.miloszgilga.htas.client.R

@Composable
fun SimpleLineChart(
  modifier: Modifier = Modifier,
  dataPoints: List<Float>,
  lineColor: Color,
) {
  val labelColor = MaterialTheme.colorScheme.onSurfaceVariant.toArgb()
  val gridColor = MaterialTheme.colorScheme.outlineVariant.copy(alpha = 0.5f).toArgb()

  val waitingForDataMsg = stringResource(R.string.no_device_paired)
  val readingsMsg = stringResource(R.string.readings)

  AndroidView(
    modifier = modifier,
    factory = { context ->
      LineChart(context).apply {
        layoutParams = LinearLayout.LayoutParams(
          ViewGroup.LayoutParams.MATCH_PARENT,
          ViewGroup.LayoutParams.MATCH_PARENT,
        )
        description.isEnabled = false
        legend.isEnabled = false
        setTouchEnabled(false)

        xAxis.position = XAxis.XAxisPosition.BOTTOM
        xAxis.setDrawGridLines(false)
        xAxis.setDrawAxisLine(false)
        xAxis.setDrawLabels(false)
        xAxis.textColor = labelColor

        axisLeft.textColor = labelColor
        axisLeft.gridColor = gridColor
        axisLeft.setDrawAxisLine(false)

        axisLeft.spaceTop = 30f
        axisLeft.spaceBottom = 30f
        axisRight.isEnabled = false

        setNoDataText("$waitingForDataMsg...")
        setNoDataTextColor(labelColor)
      }
    },
    update = { chart ->
      chart.axisLeft.textColor = labelColor
      chart.axisLeft.gridColor = gridColor
      chart.xAxis.textColor = labelColor

      if (dataPoints.isEmpty()) {
        chart.clear()
        return@AndroidView
      }
      val entries = dataPoints.mapIndexed { index, value ->
        Entry(index.toFloat(), value)
      }
      val dataSet = LineDataSet(entries, readingsMsg).apply {
        color = lineColor.toArgb()

        if (entries.size == 1) {
          setDrawCircles(true)
          setCircleColor(lineColor.toArgb())
          circleRadius = 4f
        } else {
          setDrawCircles(false)
        }
        setDrawValues(false)

        lineWidth = 2.5f
        mode = LineDataSet.Mode.CUBIC_BEZIER

        setDrawFilled(true)
        fillColor = lineColor.toArgb()
        fillAlpha = 40

        isHighlightEnabled = false
      }
      val lineData = LineData(dataSet)
      chart.data = lineData
      chart.notifyDataSetChanged()
      chart.postInvalidate()
    }
  )
}
