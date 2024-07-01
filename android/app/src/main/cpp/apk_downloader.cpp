#include <jni.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

std::string storedUrl;
std::string storedOutputFile;

extern "C" {

struct ProgressCallbackInfo {
    JNIEnv* env;
    jobject progressCallback;
    jobject onCompleteCallback;
};

int createSocket(const std::string& host, int port) {
    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        std::cerr << "No such host: " << host << std::endl;
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(sockfd);
        return -1;
    }

    return sockfd;
}

SSL_CTX* createSSLContext() {
    OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);

    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Unable to create SSL context" << std::endl;
        ERR_print_errors_fp(stderr);
    }
    return ctx;
}

void downloadFile(SSL* ssl, const std::string& path, std::ofstream& file, ProgressCallbackInfo* callbackInfo) {
    char buffer[4096];
    int bytesRead;
    double totalBytesRead = 0;

    while ((bytesRead = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
        file.write(buffer, bytesRead);
        totalBytesRead += bytesRead;

        JNIEnv* env = callbackInfo->env;
        jobject progressCallback = callbackInfo->progressCallback;
        jclass callbackClass = env->GetObjectClass(progressCallback);
        jmethodID onProgressMethod = env->GetMethodID(callbackClass, "onProgress", "(I)V");

        if (onProgressMethod) {
            int progress = static_cast<int>((totalBytesRead / 1000000) * 100);
            env->CallVoidMethod(progressCallback, onProgressMethod, progress);
        }
    }
}

JNIEXPORT void JNICALL
Java_io_github_troppical_network_APKDownloader_setUrl(JNIEnv* env, jobject thiz, jstring url) {
    const char* cUrl = env->GetStringUTFChars(url, nullptr);
    storedUrl = std::string(cUrl);
    env->ReleaseStringUTFChars(url, cUrl);
}

JNIEXPORT void JNICALL
Java_io_github_troppical_network_APKDownloader_setOutputFile(JNIEnv* env, jobject thiz, jstring outputFile) {
    const char* cOutputFile = env->GetStringUTFChars(outputFile, nullptr);
    storedOutputFile = std::string(cOutputFile);
    env->ReleaseStringUTFChars(outputFile, cOutputFile);
}

JNIEXPORT jboolean JNICALL
Java_io_github_troppical_network_APKDownloader_download(JNIEnv* env, jobject thiz, jobject progressCallback, jobject onCompleteCallback) {
    std::string urlStr = storedUrl;
    std::string outputFileStr = storedOutputFile;
    std::string protocol, host, path;
    int port = 443;

    size_t pos = urlStr.find("://");
    if (pos != std::string::npos) {
        protocol = urlStr.substr(0, pos);
        urlStr = urlStr.substr(pos + 3);
    }

    pos = urlStr.find('/');
    if (pos != std::string::npos) {
        host = urlStr.substr(0, pos);
        path = urlStr.substr(pos);
    } else {
        host = urlStr;
        path = "/";
    }

    pos = host.find(':');
    if (pos != std::string::npos) {
        port = std::stoi(host.substr(pos + 1));
        host = host.substr(0, pos);
    }

    int sockfd = createSocket(host, port);
    if (sockfd < 0) {
        return JNI_FALSE;
    }

    SSL_CTX* ctx = createSSLContext();
    if (!ctx) {
        close(sockfd);
        return JNI_FALSE;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sockfd);
        return JNI_FALSE;
    }

    std::ofstream file(outputFileStr, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputFileStr << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sockfd);
        return JNI_FALSE;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
    SSL_write(ssl, request.c_str(), request.size());

    ProgressCallbackInfo callbackInfo{env, progressCallback, onCompleteCallback};
    downloadFile(ssl, path, file, &callbackInfo);

    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);
    file.close();

    jclass callbackClass = env->GetObjectClass(onCompleteCallback);
    jmethodID onCompleteMethod = env->GetMethodID(callbackClass, "onComplete", "(Z)V");
    env->CallVoidMethod(onCompleteCallback, onCompleteMethod, JNI_TRUE);

    return JNI_TRUE;
}

}
