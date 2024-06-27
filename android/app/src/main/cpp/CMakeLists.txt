cmake_minimum_required(VERSION 3.10.2)
project(troppical)

set(CMAKE_CXX_STANDARD 11)

add_library(apkdownloader SHARED apk_downloader.cpp)

pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

add_subdirectory(libraries/libcurl)
include_directories(${CMAKE_SOURCE_DIR}/include)
target_link_libraries(apkdownloader curl)

find_library(log-lib log)
target_link_libraries(apkdownloader ${log-lib})
