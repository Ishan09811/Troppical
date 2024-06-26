#include <jni.h>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <iostream>

extern "C" {

struct ProgressCallbackInfo {
    JNIEnv* env;
    jobject progressCallback;
    jobject onCompleteCallback;
};

size_t writeData(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ofstream* file = static_cast<std::ofstream*>(stream);
    file->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int progressCallback(void* ptr, double totalToDownload, double nowDownloaded, double, double) {
    ProgressCallbackInfo* callbackInfo = static_cast<ProgressCallbackInfo*>(ptr);
    JNIEnv* env = callbackInfo->env;
    jobject progressCallback = callbackInfo->progressCallback;
    jclass callbackClass = env->GetObjectClass(progressCallback);
    jmethodID onProgressMethod = env->GetMethodID(callbackClass, "onProgress", "(I)V");

    if (totalToDownload > 0 && onProgressMethod) {
        int progress = static_cast<int>((nowDownloaded / totalToDownload) * 100);
        env->CallVoidMethod(progressCallback, onProgressMethod, progress);
    }
    return 0;
}

JNIEXPORT jboolean JNICALL
Java_io_github_troppical_network_APKDownloader_download(JNIEnv* env, jobject, jstring url, jstring outputFile, jobject progressCallback, jobject onCompleteCallback) {
    const char* cUrl = env->GetStringUTFChars(url, nullptr);
    const char* cOutputFile = env->GetStringUTFChars(outputFile, nullptr);

    CURL* curl;
    CURLcode res;
    std::ofstream file;
    ProgressCallbackInfo callbackInfo{env, progressCallback, onCompleteCallback};

    curl = curl_easy_init();
    if (curl) {
        file.open(cOutputFile, std::ios::binary);

        curl_easy_setopt(curl, CURLOPT_URL, cUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &callbackInfo);

        res = curl_easy_perform(curl);

        file.close();
        curl_easy_cleanup(curl);
        
        env->ReleaseStringUTFChars(url, cUrl);
        env->ReleaseStringUTFChars(outputFile, cOutputFile);
        
        // Call onComplete callback
        jclass callbackClass = env->GetObjectClass(onCompleteCallback);
        jmethodID onCompleteMethod = env->GetMethodID(callbackClass, "onComplete", "(Z)V");
        env->CallVoidMethod(onCompleteCallback, onCompleteMethod, res == CURLE_OK ? JNI_TRUE : JNI_FALSE);
        
        return res == CURLE_OK ? JNI_TRUE : JNI_FALSE;
    }

    env->ReleaseStringUTFChars(url, cUrl);
    env->ReleaseStringUTFChars(outputFile, cOutputFile);
    return JNI_FALSE;
}

}
