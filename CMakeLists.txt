cmake_minimum_required(VERSION 3.28)
project(framescout VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

# Find/fetch OpenCV
find_package(OpenCV 4 QUIET COMPONENTS core imgproc imgcodecs videoio)
if(NOT OpenCV_FOUND)
  include(FetchContent)

  # Skip extra builds/optimize time (https://docs.opencv.org/4.x/)
  set(BUILD_LIST core,imgproc,imgcodecs,videoio CACHE STRING "" FORCE)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
  set(BUILD_opencv_world OFF CACHE BOOL "" FORCE)
  set(BUILD_opencv_videoio_plugins OFF CACHE BOOL "" FORCE)

  set(WITH_IPP OFF CACHE BOOL "" FORCE)
  set(WITH_TBB OFF CACHE BOOL "" FORCE)
  set(WITH_OPENCL OFF CACHE BOOL "" FORCE)
  set(WITH_CUDA OFF CACHE BOOL "" FORCE)
  set(WITH_VULKAN OFF CACHE BOOL "" FORCE)
  set(WITH_EIGEN OFF CACHE BOOL "" FORCE)
  set(WITH_OPENMP OFF CACHE BOOL "" FORCE)
  set(WITH_ITT OFF CACHE BOOL "" FORCE)

  set(WITH_JPEG ON CACHE BOOL "" FORCE)
  set(WITH_PNG ON CACHE BOOL "" FORCE)
  set(WITH_TIFF OFF CACHE BOOL "" FORCE)
  set(WITH_WEBP OFF CACHE BOOL "" FORCE)
  set(WITH_OPENJPEG OFF CACHE BOOL "" FORCE)
  set(WITH_OPENEXR OFF CACHE BOOL "" FORCE)

  set(BUILD_JPEG ON CACHE BOOL "" FORCE)
  set(BUILD_PNG ON CACHE BOOL "" FORCE)
  set(BUILD_TIFF OFF CACHE BOOL "" FORCE)
  set(BUILD_WEBP OFF CACHE BOOL "" FORCE)
  set(BUILD_OPENJPEG OFF CACHE BOOL "" FORCE)
  set(BUILD_ZLIB ON CACHE BOOL "" FORCE)

  set(WITH_FFMPEG ON CACHE BOOL "" FORCE)
  set(WITH_GSTREAMER OFF CACHE BOOL "" FORCE)
  set(WITH_MSMF OFF CACHE BOOL "" FORCE)
  set(WITH_V4L ON CACHE BOOL "" FORCE)

  FetchContent_Declare(
    opencv
    URL https://github.com/opencv/opencv/archive/refs/tags/4.12.0.zip
  )

  FetchContent_MakeAvailable(opencv)
endif()

include(CTest)

add_subdirectory(src)
add_subdirectory(tests)
