add_library(vfm_cli
    vfm_args.cpp
)

target_include_directories(vfm_cli PUBLIC ./)

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
