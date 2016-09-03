
project(CPP_MCTS)
add_library(CPP_MCTS INTERFACE)
target_compile_features(CPP_MCTS INTERFACE cxx_override cxx_auto_type cxx_constexpr cxx_range_for)
target_include_directories(CPP_MCTS INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES mcts.h graphviz.h
        DESTINATION include)