#include <jni.h>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <iostream>

extern "C" {

size_t writeData(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ofstream* file = static_cast<std::ofstream*>(stream);
    file->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int progressCallback(void* ptr, double totalToDownload, double nowDownloaded, double, double) {
    JNIEnv* env = static_cast<JNIEnv*>(ptr);
    jclass callbackClass = env->FindClass("io/github/troppical/network/APKDownloader$ProgressCallback");
    jmethodID onProgressMethod = env->GetMethodID(callbackClass, "onProgress", "(I)V");

    if (totalToDownload > 0 && onProgressMethod) {
        int progress = static_cast<int>((nowDownloaded / totalToDownload) * 100);
        jobject callbackObj = env->AllocObject(callbackClass);
        env->CallVoidMethod(callbackObj, onProgressMethod, progress);
        env->DeleteLocalRef(callbackObj);
    }
    return 0;
}

JNIEXPORT jboolean JNICALL
Java_io_github_troppical_network_APKDownloader_download(JNIEnv* env, jobject, jstring url, jstring outputFile, jobject progressCallback) {
    const char* cUrl = env->GetStringUTFChars(url, nullptr);
    const char* cOutputFile = env->GetStringUTFChars(outputFile, nullptr);

    CURL* curl;
    CURLcode res;
    std::ofstream file;

    curl = curl_easy_init();
    if (curl) {
        file.open(cOutputFile, std::ios::binary);

        curl_easy_setopt(curl, CURLOPT_URL, cUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, env);

        res = curl_easy_perform(curl);

        file.close();
        curl_easy_cleanup(curl);
        
        env->ReleaseStringUTFChars(url, cUrl);
        env->ReleaseStringUTFChars(outputFile, cOutputFile);
        
        return res == CURLE_OK ? JNI_TRUE : JNI_FALSE;
    }

    env->ReleaseStringUTFChars(url, cUrl);
    env->ReleaseStringUTFChars(outputFile, cOutputFile);
    return JNI_FALSE;
}

}
