cmake_minimum_required (VERSION 3.1) 

set(CPP_MCTS_BUILD_SAMPLES ON CACHE BOOL "Build the sample applications")

add_subdirectory(src)

if (CPP_MCTS_BUILD_SAMPLES)
	add_subdirectory(samples)
endif (CPP_MCTS_BUILD_SAMPLES)