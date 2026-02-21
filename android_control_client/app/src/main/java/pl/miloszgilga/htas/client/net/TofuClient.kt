package pl.miloszgilga.htas.client.net

import android.annotation.SuppressLint
import android.util.Log
import okhttp3.OkHttpClient
import okio.ByteString.Companion.toByteString
import java.security.SecureRandom
import java.security.cert.CertificateException
import java.security.cert.X509Certificate
import java.util.concurrent.TimeUnit
import javax.net.ssl.SSLContext
import javax.net.ssl.X509TrustManager

sealed class TofuCertificateException(message: String) : CertificateException(message) {
  class NoCertificate : TofuCertificateException("no certificate provided by the server")
  class HashMismatch(val expected: String, val actual: String) :
    TofuCertificateException("certificate hash mismatch; expected: $expected, actual: $actual")
}

class TofuClient(expectedSha256Hash: String) {
  companion object {
    private const val TAG = "TofuClient"

    private const val TIMEOUT_SEC = 30L
  }

  @SuppressLint("CustomX509TrustManager")
  private val trustManager = object : X509TrustManager {
    @SuppressLint("TrustAllX509TrustManager")
    override fun checkClientTrusted(chain: Array<out X509Certificate>?, authType: String?) {
    }

    override fun checkServerTrusted(chain: Array<out X509Certificate>?, authType: String?) {
      Log.d(TAG, "checking server trust, auth type: $authType")

      if (chain.isNullOrEmpty()) {
        throw TofuCertificateException.NoCertificate()
      }
      val serverCert = chain[0]
      val serverHash = serverCert.encoded.toByteString().sha256().hex().lowercase()
      if (serverHash != expectedSha256Hash) {
        Log.e(TAG, "hash mismatch, expected: $expectedSha256Hash, actual: $serverHash")
        throw TofuCertificateException.HashMismatch(expectedSha256Hash, serverHash)
      }
      Log.d(TAG, "server certificate verified successfully against expected hash")
    }

    override fun getAcceptedIssuers(): Array<X509Certificate> = arrayOf()
  }

  fun getClient(): OkHttpClient {
    val sslContext = SSLContext.getInstance("TLS")
    sslContext.init(null, arrayOf(trustManager), SecureRandom())
    return OkHttpClient.Builder()
      .connectTimeout(TIMEOUT_SEC, TimeUnit.SECONDS)
      .readTimeout(TIMEOUT_SEC, TimeUnit.SECONDS)
      .writeTimeout(TIMEOUT_SEC, TimeUnit.SECONDS)
      .sslSocketFactory(sslContext.socketFactory, trustManager)
      .hostnameVerifier { hostname, _ ->
        Log.d(TAG, "bypassing standard hostname verification for: $hostname")
        true
      }
      .build()
  }
}
