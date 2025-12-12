add_library(vfm_io
	vfm_io.cpp
)

target_include_directories(vfm_io PUBLIC
	./
	${CMAKE_CURRENT_SOURCE_DIR}/../core
)
