include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(test_vfm)
target_link_libraries(test_vfm
	PUBLIC
		GTest::gtest_main
		vfm_cli
		vfm_core
)
target_include_directories(test_vfm PUBLIC ./)

target_sources(test_vfm
	PRIVATE
		vfm_args.t.cpp
		vfm_imagesearch.t.cpp
		vfm_testimages.cpp
)

gtest_discover_tests(test_vfm)
