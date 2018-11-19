# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)
set(lib_src_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
include_directories(
        ${CMAKE_SOURCE_DIR}/src/main/cpp/FFMPEG/include
        ${CMAKE_SOURCE_DIR}/src/main/cpp/AACDecoder
        ${CMAKE_SOURCE_DIR}/src/main/cpp/AACDecoder/faad2/include
        ${CMAKE_SOURCE_DIR}/src/main/cpp/AACDecoder/faad2/libfaad
        ${CMAKE_SOURCE_DIR}/src/main/cpp/G711Decoder
        ${CMAKE_SOURCE_DIR}/src/main/cpp/G726Decoder
        ${CMAKE_SOURCE_DIR}/src/main/cpp/OpenSSL
)

# FFMPEG
add_library(avcodec-56 SHARED IMPORTED)
set_target_properties(avcodec-56 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavcodec-56.so)

add_library(avdevice-56 SHARED IMPORTED)
set_target_properties(avdevice-56 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavdevice-56.so)

add_library(avfilter-5 SHARED IMPORTED)
set_target_properties(avfilter-5 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavfilter-5.so)

add_library(avformat-56 SHARED IMPORTED)
set_target_properties(avformat-56 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavformat-56.so)

add_library(avutil-54 SHARED IMPORTED)
set_target_properties(avutil-54 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavutil-54.so)

add_library(swresample-1 SHARED IMPORTED)
set_target_properties(swresample-1 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswresample-1.so)

add_library(swscale-3 SHARED IMPORTED)
set_target_properties(swscale-3 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswscale-3.so)

# FAAD
add_library(faad2 SHARED IMPORTED)
set_target_properties(faad2 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libfaad2.so)

# G711Decoder
add_library(g711 SHARED IMPORTED)
set_target_properties(g711 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libG711Decoder.so)

# G726Decoder
add_library(g726 SHARED IMPORTED)
set_target_properties(g726 PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libG726Decoder.so)

# OpenSSL
add_library(crypto STATIC IMPORTED)
set_target_properties(  crypto
                        PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libcrypto.a)

add_library(ssl STATIC IMPORTED)
set_target_properties(  ssl
                        PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libssl.a)

add_library(
             DLinkCrypto
             SHARED
             src/main/cpp/OpenSSL/DLinkCrypto.cpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             DecoderInterface

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/FFMPEG/DecoderInterface.c)

add_library(
             AACDecoder
             SHARED
             src/main/cpp/AACDecoder/AACDecoder.cpp)


add_library(
             G711Decoder
             SHARED
             src/main/cpp/G711Decoder/G711Decoder.cpp)

add_library(
             G726Decoder
             SHARED
             src/main/cpp/G726Decoder/G726Decoder.cpp)

add_library(
             RtspConnector
             SHARED
             src/main/cpp/FFMPEG/RtspConnector.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       DecoderInterface

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        android
                        jnigraphics
                        avformat-56
                        avdevice-56
                        avcodec-56
                        avfilter-5
                        avutil-54
                        swresample-1
                        swscale-3
)

target_link_libraries(
                        AACDecoder
                        ${log-lib}
                        android
                        faad2
)

target_link_libraries(
                        G711Decoder
                        ${log-lib}
                        android
                        g711
)

target_link_libraries(
                        G726Decoder
                        ${log-lib}
                        android
                        g726
)

target_link_libraries( # Specifies the target library.
                       RtspConnector

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        android
                        jnigraphics
                        avformat-56
                        avdevice-56
                        avcodec-56
                        avfilter-5
                        avutil-54
                        swresample-1
                        swscale-3
)

target_link_libraries( # Specifies the target library.
                       DLinkCrypto

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        android
                        crypto
                        ssl
)
