project(CPP_MCTS)

cmake_minimum_required (VERSION 3.15)

include(CTest)

set(CPP_MCTS_BUILD_SAMPLES ON CACHE BOOL "Build the sample applications")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_library(cpp_mcts INTERFACE)
target_compile_features(cpp_mcts INTERFACE cxx_override cxx_auto_type cxx_constexpr cxx_range_for)
target_include_directories(cpp_mcts INTERFACE include)
set_target_properties(cpp_mcts PROPERTIES PUBLIC_HEADER "include/mcts/mcts.hpp;include/mcts/graphviz.hpp")
install(TARGETS cpp_mcts PUBLIC_HEADER DESTINATION include/mcts)

if (CPP_MCTS_BUILD_SAMPLES)
	add_subdirectory(samples)
endif (CPP_MCTS_BUILD_SAMPLES)

if (BUILD_TESTING)
	add_subdirectory(test)
endif (BUILD_TESTING)
