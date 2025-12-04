target_include_directories(vfm_core PUBLIC ./)

target_sources(vfm_core
 	PRIVATE
		vfm_imagesearch.cpp
)

# Find/fetch OpenCV
find_package(OpenCV 4 QUIET COMPONENTS core imgproc imgcodecs videoio)
if(OpenCV_FOUND)
	target_link_libraries(vfm_core PUBLIC ${OpenCV_LIBS})
	target_include_directories(vfm_core PUBLIC ${OpenCV_INCLUDE_DIRS})
else()
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

    target_link_libraries(vfm_core PUBLIC
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_videoio
    )
    target_include_directories(vfm_core PUBLIC
        ${opencv_SOURCE_DIR}/include
        ${opencv_SOURCE_DIR}/modules/core/include
        ${opencv_SOURCE_DIR}/modules/imgproc/include
        ${opencv_SOURCE_DIR}/modules/imgcodecs/include
        ${opencv_SOURCE_DIR}/modules/videoio/include
        ${opencv_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
    )
endif()
