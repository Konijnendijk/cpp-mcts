cmake_minimum_required (VERSION 3.0) 
project(CPP_MCTS)
add_library(CPP_MCTS INTERFACE)
target_compile_features(CPP_MCTS INTERFACE cxx_override cxx_auto_type cxx_constexpr cxx_range_for)