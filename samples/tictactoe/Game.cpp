
#include <QApplication>

#include "GUI.h"

/*
int main(int argc, char **argv){
	Board board;
	TTTMCTSPlayer p=TTTMCTSPlayer(&board);

	int x, y;
	while (board.won()==NONE && board.getTurns() < 9){
		// print the board
		for (int y = 0; y < 3; y++){
			for (int x = 0; x < 3; x++)
				std::cout << Board::playerToChar(board.position(x, y));
			std::cout << "" << std::endl;
		}
		std::cout << std::endl;


		// Human player goes first, MCTS second.
		if (board.getTurns()%2==0){
//			std::cin >> x >> y;
//
//			board.play(x, y);
			p.play();
		}
		else{
			p.play();
		}
	}

	std::cout << Board::playerToChar(board.won()) << " won the game" << std::endl;

	std::cin >> x;

	return 0;

	
}*/


int main(int argc, char **argv){
	QApplication app (argc, argv);

	GUI gui;
	gui.show();

	return app.exec();
}

