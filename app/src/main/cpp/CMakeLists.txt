# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

set(lib_src_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

# build application's shared lib

add_library(videoplay SHARED
            videoplay.cpp)



add_library(avformat-56_lib SHARED IMPORTED)
set_target_properties(avformat-56_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavformat-56.so)

add_library(avutil-54_lib SHARED IMPORTED)
set_target_properties(avutil-54_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavutil-54.so)

add_library(swresample-1_lib SHARED IMPORTED)
set_target_properties(swresample-1_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswresample-1.so)

add_library(swscale-3_lib SHARED IMPORTED)
set_target_properties(swscale-3_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswscale-3.so)

add_library(avcodec-56_lib SHARED IMPORTED)
set_target_properties(avcodec-56_lib PROPERTIES IMPORTED_LOCATION
                             ${lib_src_DIR}/libavcodec-56.so)

add_library(H264Codec_lib SHARED IMPORTED)
set_target_properties(H264Codec_lib  PROPERTIES IMPORTED_LOCATION
                             ${lib_src_DIR}/libwzh033.so)




# Include libraries needed for videoplay lib
target_link_libraries(
    videoplay
    log
    android
    avcodec-56_lib
    avformat-56_lib
    avutil-54_lib
    swresample-1_lib
    swscale-3_lib)