cmake_minimum_required (VERSION 3.0)

# Qt related
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core)
find_package(Qt5Widgets)

if(Qt5Core_FOUND AND Qt5Widgets_FOUND)
	add_executable(cpp_mcts_tictactoe Game.cpp Board.h Board.cpp TTTAction.h TTTStrategy.h TTTStrategy.cpp TTTMCTSPlayer.h TTTMCTSPlayer.cpp GUI.h GUI.cpp)
	target_compile_features(cpp_mcts_tictactoe PRIVATE cxx_override cxx_range_for)
	target_link_libraries(cpp_mcts_tictactoe CPP_MCTS Qt5::Core Qt5::Widgets)
endif()