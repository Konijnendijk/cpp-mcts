
# Qt related
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core)
find_package(Qt5Widgets)

if(Qt5Core_FOUND AND Qt5Widgets_FOUND)
	add_executable(cpp_mcts_tictactoe Game.cpp Board.cpp TTTStrategy.cpp TTTMCTSPlayer.cpp GUI.hpp GUI.cpp)
	target_compile_features(cpp_mcts_tictactoe PRIVATE cxx_override cxx_range_for)
	target_link_libraries(cpp_mcts_tictactoe cpp_mcts Qt5::Core Qt5::Widgets)
endif()