cmake_minimum_required(VERSION 3.10.2)
project(troppical)

set(CMAKE_CXX_STANDARD 11)

add_library(apkdownloader SHARED apk_downloader.cpp)

find_package(OpenSSL REQUIRED)

add_subdirectory(libraries/libcurl)
include_directories(${CMAKE_SOURCE_DIR}/include)

if (TARGET libcurl)
    set(CURL_LIB libcurl)
else()
    find_package(CURL REQUIRED)
    set(CURL_LIB ${CURL_LIBRARIES})
endif()

target_link_libraries(apkdownloader ${CURL_LIB})

find_library(log-lib log)
target_link_libraries(apkdownloader ${log-lib})
