include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(test_framescout)
target_link_libraries(test_framescout
	PUBLIC
		GTest::gtest_main
		framescout_core
)

target_sources(test_framescout
	PRIVATE
		framescout_imagesearch.t.cpp
)

gtest_discover_tests(test_framescout)
