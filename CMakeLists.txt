project(CPP_MCTS)

cmake_minimum_required (VERSION 3.1)

include(CTest)

set(CPP_MCTS_BUILD_SAMPLES ON CACHE BOOL "Build the sample applications")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_library(CPP_MCTS INTERFACE)
target_compile_features(CPP_MCTS INTERFACE cxx_override cxx_auto_type cxx_constexpr cxx_range_for)
target_include_directories(CPP_MCTS INTERFACE include)
set_target_properties(CPP_MCTS PROPERTIES PUBLIC_HEADER "include/mcts/mcts.h;include/mcts/graphviz.h")
install(TARGETS CPP_MCTS PUBLIC_HEADER DESTINATION include/mcts)

if (CPP_MCTS_BUILD_SAMPLES)
	add_subdirectory(samples)
endif (CPP_MCTS_BUILD_SAMPLES)

if (BUILD_TESTING)
	add_subdirectory(test)
endif (BUILD_TESTING)
