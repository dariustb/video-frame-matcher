target_include_directories(vfm_cli PUBLIC ./)

target_sources(vfm_cli
 	PRIVATE
		vfm_args.cpp
)

# Fetch CLI11
include(FetchContent)

FetchContent_Declare(
    cli11
    URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.6.1.zip
)
FetchContent_MakeAvailable(cli11)

target_link_libraries(vfm_cli PUBLIC
    CLI11::CLI11
)

target_include_directories(vfm_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
