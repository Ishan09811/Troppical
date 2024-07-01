package io.github.troppical.network

class APKDownloader() {

    init {
        System.loadLibrary("apkdownloader")
    }
    
    external fun setUrl(url: String)
    external fun setOutputFile(outputFile: String)
    external fun download(progressCallback: ProgressCallback, onCompleteCallback: OnCompleteCallback): Boolean

    interface ProgressCallback {
        fun onProgress(progress: Int)
    }

    interface OnCompleteCallback {
        fun onComplete(success: Boolean)
    }
}
