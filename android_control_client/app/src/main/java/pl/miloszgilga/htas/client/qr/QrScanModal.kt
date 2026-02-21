package pl.miloszgilga.htas.client.qr

import android.Manifest
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.min
import androidx.compose.ui.window.Dialog
import androidx.compose.ui.window.DialogProperties
import com.google.accompanist.permissions.ExperimentalPermissionsApi
import com.google.accompanist.permissions.isGranted
import com.google.accompanist.permissions.rememberPermissionState
import pl.miloszgilga.htas.client.R
import pl.miloszgilga.htas.client.composable.button.AppOutlinedButton
import pl.miloszgilga.htas.client.toast.ToastManager
import pl.miloszgilga.htas.client.toast.ToastType
import pl.miloszgilga.htas.client.viewmodel.MainViewModel

@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun QrScanModal(
  viewModel: MainViewModel,
  onDismiss: () -> Unit,
  onCodeScanned: (String) -> Unit,
) {
  val noCameraPermissionsMsg = stringResource(R.string.no_camera_permissions)
  val cameraPermissionState = rememberPermissionState(Manifest.permission.CAMERA) { isGranted ->
    if (!isGranted) {
      ToastManager.show(noCameraPermissionsMsg, ToastType.ERROR)
      onDismiss()
    }
  }

  LaunchedEffect(Unit) {
    if (!cameraPermissionState.status.isGranted) {
      cameraPermissionState.launchPermissionRequest()
    }
  }

  Dialog(
    onDismissRequest = onDismiss,
    properties = DialogProperties(
      usePlatformDefaultWidth = false,
      decorFitsSystemWindows = false,
    )
  ) {
    BoxWithConstraints(
      modifier = Modifier.fillMaxSize(),
      contentAlignment = Alignment.Center,
    ) {
      Card(
        modifier = Modifier
          .size(min(maxWidth, maxHeight) * 0.85f)
          .padding(16.dp),
        shape = RoundedCornerShape(24.dp),
        colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surface),
      ) {
        Box(contentAlignment = Alignment.BottomCenter) {
          Column(
            modifier = Modifier
              .fillMaxSize()
              .padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
          ) {
            Text(
              text = stringResource(R.string.scan_qr_code),
              style = MaterialTheme.typography.titleMedium,
              modifier = Modifier.padding(bottom = 8.dp),
            )
            Box(
              modifier = Modifier
                .weight(1f)
                .fillMaxWidth()
                .clip(RoundedCornerShape(16.dp))
                .background(MaterialTheme.colorScheme.onSurface.copy(alpha = 0.1f)),
            ) {
              if (cameraPermissionState.status.isGranted) {
                CameraPreviewView(
                  onScan = { code ->
                    if (code.isNotEmpty()) {
                      onCodeScanned(code)
                    }
                  },
                  onError = { errorMsg ->
                    onDismiss()
                    viewModel.onQrScanFailure(errorMsg)
                  }
                )
              } else {
                Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                  Text(
                    text = "${stringResource(R.string.permissions_required)}...",
                    style = MaterialTheme.typography.bodySmall,
                  )
                }
              }
            }
            Spacer(modifier = Modifier.height(16.dp))
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
              AppOutlinedButton(
                text = stringResource(R.string.cancel),
                onClick = onDismiss,
                modifier = Modifier.weight(1f),
              )
            }
          }
        }
      }
    }
  }
}
