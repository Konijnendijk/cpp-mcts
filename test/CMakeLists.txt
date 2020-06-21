

add_executable(cpp_mcts_tests main.cpp)
target_link_libraries(cpp_mcts_tests PRIVATE CONAN_PKG::catch2)
