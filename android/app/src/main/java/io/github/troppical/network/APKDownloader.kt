package io.github.troppical.network

class APKDownloader(private val url: String, private val outputFile: String) {

    init {
        System.loadLibrary("apkdownloader")
    }

    external fun download(progressCallback: ProgressCallback): Boolean

    interface ProgressCallback {
        fun onProgress(progress: Int)
    }
}
