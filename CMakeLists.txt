project(CPP_MCTS)

cmake_minimum_required (VERSION 3.1)

include(CTest)

set(CPP_MCTS_BUILD_SAMPLES ON CACHE BOOL "Build the sample applications")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)

if (CPP_MCTS_BUILD_SAMPLES)
	add_subdirectory(samples)
endif (CPP_MCTS_BUILD_SAMPLES)

if (BUILD_TESTING)
	add_subdirectory(test)
endif (BUILD_TESTING)
