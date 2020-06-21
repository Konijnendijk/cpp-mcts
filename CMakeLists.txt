project(CPP_MCTS)

cmake_minimum_required (VERSION 3.1)

set(CPP_MCTS_BUILD_SAMPLES ON CACHE BOOL "Build the sample applications")
set(CPP_MCTS_BUILD_TESTS ON CACHE BOOL "Build the tests")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)

if (CPP_MCTS_BUILD_SAMPLES)
	add_subdirectory(samples)
endif (CPP_MCTS_BUILD_SAMPLES)

if (CPP_MCTS_BUILD_TESTS)
	add_subdirectory(test)
endif (CPP_MCTS_BUILD_TESTS)
