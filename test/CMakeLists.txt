

add_executable(cpp_mcts_tests main.cpp node.cpp)
target_link_libraries(cpp_mcts_tests PRIVATE CONAN_PKG::catch2 CPP_MCTS)

# Instrument for code coverage
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(cpp_mcts_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping -mllvm -enable-name-compression=false)
    target_link_options(cpp_mcts_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
endif()

include(Catch)
catch_discover_tests(cpp_mcts_tests)
