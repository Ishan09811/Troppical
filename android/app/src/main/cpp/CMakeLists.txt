cmake_minimum_required(VERSION 3.10.2)
project(troppical)

set(CMAKE_CXX_STANDARD 11)

add_library(apkdownloader SHARED apk_downloader.cpp)

add_subdirectory(libraries/libcurl)
include_directories(${CMAKE_SOURCE_DIR}/include)
target_link_libraries(apkdownloader curl)

find_library(log-lib log)
target_link_libraries(apkdownloader ${log-lib})
