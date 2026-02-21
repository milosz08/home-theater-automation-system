package pl.miloszgilga.htas.client.qr

import android.content.Context
import android.view.ViewGroup
import androidx.annotation.OptIn
import androidx.camera.core.CameraSelector
import androidx.camera.core.ExperimentalGetImage
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import androidx.lifecycle.compose.LocalLifecycleOwner
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

@OptIn(ExperimentalGetImage::class)
@Composable
fun CameraPreviewView(
  onScan: (String) -> Unit,
  onError: (String?) -> Unit,
) {
  val context = LocalContext.current
  val lifecycleOwner = LocalLifecycleOwner.current
  val cameraExecutor = remember { Executors.newSingleThreadExecutor() }

  AndroidView(
    modifier = Modifier.fillMaxSize(),
    factory = { ctx -> createPreviewView(ctx) },
    update = { previewView ->
      bindCameraUseCases(
        context = context,
        lifecycleOwner = lifecycleOwner,
        previewView = previewView,
        cameraExecutor = cameraExecutor,
        onScan = onScan,
        onError = onError,
      )
    }
  )
}

private fun createPreviewView(context: Context): PreviewView {
  return PreviewView(context).apply {
    implementationMode = PreviewView.ImplementationMode.COMPATIBLE
    scaleType = PreviewView.ScaleType.FILL_CENTER
    layoutParams = ViewGroup.LayoutParams(
      ViewGroup.LayoutParams.MATCH_PARENT,
      ViewGroup.LayoutParams.MATCH_PARENT,
    )
  }
}

private fun bindCameraUseCases(
  context: Context,
  lifecycleOwner: LifecycleOwner,
  previewView: PreviewView,
  cameraExecutor: ExecutorService,
  onScan: (String) -> Unit,
  onError: (String?) -> Unit,
) {
  val cameraProviderFuture = ProcessCameraProvider.getInstance(context)
  cameraProviderFuture.addListener({
    val cameraProvider = cameraProviderFuture.get()
    val preview = Preview.Builder().build().also {
      it.surfaceProvider = previewView.surfaceProvider
    }
    val imageAnalyzer = ImageAnalysis.Builder()
      .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
      .build()
      .also {
        it.setAnalyzer(cameraExecutor, QrCodeAnalyzer(onScan))
      }
    try {
      cameraProvider.unbindAll()
      cameraProvider.bindToLifecycle(
        lifecycleOwner,
        CameraSelector.DEFAULT_BACK_CAMERA,
        preview,
        imageAnalyzer
      )
    } catch (ex: Exception) {
      onError(ex.message)
    }
  }, ContextCompat.getMainExecutor(context))
}
