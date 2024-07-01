package io.github.troppical.network

class APKDownloader(val url: String, val outputFile: String) {

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
