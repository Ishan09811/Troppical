package io.github.troppical.network

class APKDownloader(private val url: String, private val outputFile: String) {

    init {
        System.loadLibrary("apkdownloader")
    }

    external fun download(progressCallback: ProgressCallback, onCompleteCallback: OnCompleteCallback): Boolean

    interface ProgressCallback {
        fun onProgress(progress: Int)
    }

    interface OnCompleteCallback {
        fun onComplete(success: Boolean)
    }
}
