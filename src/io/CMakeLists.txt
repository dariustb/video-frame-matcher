target_include_directories(vfm_io PUBLIC
	./
	${CMAKE_CURRENT_SOURCE_DIR}/../core
)

target_sources(vfm_io
 	PRIVATE
		vfm_io.cpp
)
