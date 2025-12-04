set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
        framescout
        framescout.m.cpp
)

add_library(framescout_core)
add_subdirectory(core)

if(OpenCV_FOUND)
    target_link_libraries(framescout PRIVATE ${OpenCV_LIBS})
    target_include_directories(framescout PRIVATE ${OpenCV_INCLUDE_DIRS})
else()
    target_link_libraries(framescout PRIVATE
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_videoio
    )

    target_include_directories(framescout PRIVATE
        ${opencv_SOURCE_DIR}/include
        ${opencv_SOURCE_DIR}/modules/core/include
        ${opencv_SOURCE_DIR}/modules/imgproc/include
        ${opencv_SOURCE_DIR}/modules/imgcodecs/include
        ${opencv_SOURCE_DIR}/modules/videoio/include
        ${opencv_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
    )
endif()

target_link_libraries(framescout PUBLIC
    framescout_core
)
