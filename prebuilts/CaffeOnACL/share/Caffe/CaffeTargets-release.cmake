#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "caffe" for configuration "Release"
set_property(TARGET caffe APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(caffe PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "proto;/home/joy/CaffeOnACL-Android/prebuilts/boost/lib/libboost_system.a;/home/joy/CaffeOnACL-Android/prebuilts/boost/lib/libboost_thread.a;/home/joy/CaffeOnACL-Android/prebuilts/boost/lib/libboost_filesystem.a;/home/joy/CaffeOnACL-Android/prebuilts/glog/lib/libglog.a;/home/joy/CaffeOnACL-Android/prebuilts/gflags/lib/libgflags.a;/home/joy/CaffeOnACL-Android/prebuilts/protobuf/lib/libprotobuf.a;/home/joy/CaffeOnACL-Android/prebuilts/lmdb/lib/liblmdb.a;/home/joy/CaffeOnACL-Android/prebuilts/ComputeLibrary/lib/libarm_compute_core-static.a;/home/joy/CaffeOnACL-Android/prebuilts/ComputeLibrary/lib/libarm_compute-static.a;/home/joy/CaffeOnACL-Android/prebuilts/ComputeLibrary/lib/libOpenCL.so;opencv_core;opencv_highgui;opencv_imgproc;opencv_imgcodecs;/home/joy/CaffeOnACL-Android/prebuilts/openblas/lib/libopenblas.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libcaffe.so"
  IMPORTED_SONAME_RELEASE "libcaffe.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS caffe )
list(APPEND _IMPORT_CHECK_FILES_FOR_caffe "${_IMPORT_PREFIX}/lib/libcaffe.so" )

# Import target "proto" for configuration "Release"
set_property(TARGET proto APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(proto PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/home/joy/CaffeOnACL-Android/prebuilts/protobuf/lib/libprotobuf.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libproto.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS proto )
list(APPEND _IMPORT_CHECK_FILES_FOR_proto "${_IMPORT_PREFIX}/lib/libproto.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
