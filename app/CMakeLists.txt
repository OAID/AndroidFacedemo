# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6)


set(CMAKE_VERBOSE_MAKEFILE on)
set(libs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
set(facesrc "${CMAKE_SOURCE_DIR}/src/main/cpp/facecaffe")
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

add_library(libopencv_java3 SHARED IMPORTED )
set_target_properties(libopencv_java3 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libopencv_java3.so")

add_library(libOpenCL SHARED IMPORTED )
set_target_properties(libOpenCL PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libOpenCL.so")

add_library(libcaffe_jni SHARED IMPORTED )
set_target_properties(libcaffe_jni PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libcaffe_jni.so")

add_library(libcaffe SHARED IMPORTED )
set_target_properties(libcaffe PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libcaffe.so")

add_library(libarm_compute_core-static STATIC IMPORTED )
set_target_properties(libarm_compute_core-static PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libarm_compute_core-static.a")

add_library(libarm_compute-static STATIC IMPORTED )
set_target_properties(libarm_compute-static PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libarm_compute-static.a")

add_library(libboost_system STATIC IMPORTED )
set_target_properties(libboost_system PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libboost_system.a")

add_library(libglog STATIC IMPORTED )
set_target_properties(libglog PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libglog.a")

add_library(libprotobuf STATIC IMPORTED )
set_target_properties(libprotobuf PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libprotobuf.a")

add_library(libproto STATIC IMPORTED )
set_target_properties(libproto PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libproto.a")

set(LOCAL_LDLIBS  "-ltinyalsa")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY=1  -std=gnu++11 -fexceptions -frtti")

aux_source_directory(${CMAKE_SOURCE_DIR}/src/main/cpp/ DIR_LIB_SRCS)

add_library(native-lib SHARED  ${DIR_LIB_SRCS})



#target_include_directories(native-lib PRIVATE ${DIR_LIB_SRCS})

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp/facecaffe)

target_link_libraries(native-lib android log tinyalsa
    libopencv_java3  facecaffe libcaffe
    libprotobuf libproto libglog libarm_compute-static
    libarm_compute_core-static libboost_system libcaffe_jni
    ${log-lib}
    )