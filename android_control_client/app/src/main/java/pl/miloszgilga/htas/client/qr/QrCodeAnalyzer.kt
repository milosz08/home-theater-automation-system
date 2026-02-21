package pl.miloszgilga.htas.client.qr

import androidx.annotation.OptIn
import androidx.camera.core.ExperimentalGetImage
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import com.google.mlkit.vision.barcode.BarcodeScanning
import com.google.mlkit.vision.common.InputImage

class QrCodeAnalyzer(private val onQrCodeScanned: (String) -> Unit) : ImageAnalysis.Analyzer {
  private var hasScanned = false

  @OptIn(ExperimentalGetImage::class)
  override fun analyze(imageProxy: ImageProxy) {
    if (hasScanned) {
      imageProxy.close()
      return
    }
    val mediaImage = imageProxy.image
    if (mediaImage == null) {
      imageProxy.close()
      return
    }
    val image = InputImage.fromMediaImage(mediaImage, imageProxy.imageInfo.rotationDegrees)
    val scanner = BarcodeScanning.getClient()
    scanner.process(image)
      .addOnSuccessListener { barcodes ->
        val barcode = barcodes.firstOrNull()
        barcode?.rawValue?.let { codeValue ->
          if (!hasScanned) {
            hasScanned = true
            onQrCodeScanned(codeValue)
          }
        }
      }
      .addOnCompleteListener { imageProxy.close() }
      .addOnFailureListener { imageProxy.close() }
  }
}
