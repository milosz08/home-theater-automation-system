package pl.miloszgilga.htas.client.update

import com.google.gson.annotations.SerializedName

data class GithubRelease(
  @SerializedName("tag_name") val tagName: String,
  @SerializedName("assets") val assets: List<GithubAsset> = emptyList(),
)

data class GithubAsset(
  @SerializedName("name") val name: String,
  @SerializedName("browser_download_url") val downloadUrl: String,
)
