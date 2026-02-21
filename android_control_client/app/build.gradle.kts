import org.jetbrains.kotlin.gradle.dsl.JvmTarget

plugins {
  alias(libs.plugins.android.application)
  alias(libs.plugins.kotlin.android)
  alias(libs.plugins.kotlin.compose)
}

android {
  namespace = "pl.miloszgilga.htas.client"
  compileSdk {
    version = release(36)
  }

  defaultConfig {
    applicationId = "pl.miloszgilga.htas"
    minSdk = 26
    targetSdk = 36
    versionCode = 1
    versionName = "1.0"

    val appVersion = System.getenv("APP_VER") ?: "0.0.1-dev-local"
    buildConfigField("String", "CI_BUILD_VERSION", "\"$appVersion\"")
    buildConfigField("long", "BUILD_TIME", "${System.currentTimeMillis()}L")
  }

  buildTypes {
    release {
      isMinifyEnabled = false
      proguardFiles(
        getDefaultProguardFile("proguard-android-optimize.txt")
      )
    }
  }
  compileOptions {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
  }
  kotlin {
    compilerOptions {
      jvmTarget.set(JvmTarget.JVM_11)
    }
  }
  buildFeatures {
    compose = true
    buildConfig = true
  }
}

dependencies {
  implementation(libs.core.ktx)
  implementation(libs.lifecycle.runtime.ktx)
  implementation(libs.activity.compose)
  implementation(libs.datastore.preferences)

  implementation(platform(libs.compose.bom))
  implementation(libs.compose.ui)
  implementation(libs.compose.ui.graphics)
  implementation(libs.compose.ui.tooling.preview)
  implementation(libs.compose.material3)
  implementation(libs.navigation.compose)
  implementation(libs.material.icons.extended)

  implementation(libs.camera.core)
  implementation(libs.camera.camera2)
  implementation(libs.camera.lifecycle)
  implementation(libs.camera.view)
  implementation(libs.barcode.scanning)
  implementation(libs.accompanist.permissions)

  implementation(libs.okhttp)
  implementation(libs.gson)
  implementation(libs.mp.android.chart)
}
