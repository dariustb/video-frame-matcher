target_include_directories(framescout_core PUBLIC ./)

target_sources(framescout_core
 	PRIVATE
		framescout_imagesearch.cpp
)

if(OpenCV_FOUND)
	target_link_libraries(framescout_core PUBLIC ${OpenCV_LIBS})
	target_include_directories(framescout_core PUBLIC ${OpenCV_INCLUDE_DIRS})
else()
    target_link_libraries(framescout_core PUBLIC
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_videoio
    )
    target_include_directories(framescout_core PUBLIC
        ${opencv_SOURCE_DIR}/include
        ${opencv_SOURCE_DIR}/modules/core/include
        ${opencv_SOURCE_DIR}/modules/imgproc/include
        ${opencv_SOURCE_DIR}/modules/imgcodecs/include
        ${opencv_SOURCE_DIR}/modules/videoio/include
        ${opencv_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
    )
endif()
