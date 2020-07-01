
add_executable(cpp_mcts_tests Main.cpp Node.cpp TestGame.cpp)
target_link_libraries(cpp_mcts_tests PRIVATE CONAN_PKG::catch2 cpp_mcts)

# Instrument for code coverage
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(cpp_mcts_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(cpp_mcts_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
endif()

include(Catch)
catch_discover_tests(cpp_mcts_tests)
