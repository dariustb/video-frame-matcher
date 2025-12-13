add_library(vfm_json
    vfm_json.cpp
)

target_include_directories(vfm_json PUBLIC ./)

# Link to vfm_core for vfm_types.h
target_link_libraries(vfm_json PUBLIC vfm_core)

# Fetch nlohmann's json (https://github.com/nlohmann/json)
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
	target_link_libraries(vfm_json PUBLIC ${nlohmann_json_LIBS})
	target_include_directories(vfm_json PUBLIC ${nlohmann_json_INCLUDE_DIRS})
else()
    include(FetchContent)

    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.zip
    )
    FetchContent_MakeAvailable(nlohmann_json)

    target_link_libraries(vfm_json PUBLIC nlohmann_json::nlohmann_json)
endif()
