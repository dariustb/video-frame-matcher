add_subdirectory(cli)
add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(json)

add_executable(vfm
    main.cpp
)

target_link_libraries(vfm PUBLIC
    vfm_cli
    vfm_core
    vfm_io
    vfm_json
)

if(OpenCV_FOUND)
    target_link_libraries(vfm PRIVATE ${OpenCV_LIBS})
    target_include_directories(vfm PRIVATE ${OpenCV_INCLUDE_DIRS})
else()
    target_link_libraries(vfm PRIVATE
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_videoio
    )

    target_include_directories(vfm PRIVATE
        ${opencv_SOURCE_DIR}/include
        ${opencv_SOURCE_DIR}/modules/core/include
        ${opencv_SOURCE_DIR}/modules/imgproc/include
        ${opencv_SOURCE_DIR}/modules/imgcodecs/include
        ${opencv_SOURCE_DIR}/modules/videoio/include
        ${opencv_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
    )
endif()

