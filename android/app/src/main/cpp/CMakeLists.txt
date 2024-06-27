cmake_minimum_required(VERSION 3.10.2)
project(apk_downloader)

add_library(apkdownloader SHARED apk_downloader.cpp)

find_library(log-lib log)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

target_link_libraries(apkdownloader ${CURL_LIBRARIES} ${log-lib})
