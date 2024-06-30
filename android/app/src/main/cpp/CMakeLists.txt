cmake_minimum_required(VERSION 3.10.2)
project(troppical)

set(CMAKE_CXX_STANDARD 11)

add_library(apkdownloader SHARED apk_downloader.cpp)

add_subdirectory(libraries/openssl-cmake)

# Include directories for OpenSSL
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link against OpenSSL libraries
target_link_libraries(apkdownloader OpenSSL::SSL OpenSSL::Crypto)

# Link against Android log library
find_library(log-lib log)
target_link_libraries(apkdownloader ${log-lib})
