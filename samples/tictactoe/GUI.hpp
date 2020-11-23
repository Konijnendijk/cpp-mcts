/*
 * GUI.h
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#ifndef CPP_MCTS_GUI_HPP
#define CPP_MCTS_GUI_HPP

#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "Board.hpp"
#include "TTTMCTSPlayer.hpp"

class GUI : public QWidget {
    Q_OBJECT

    constexpr static qreal SCENE_SIZE = 500;
    constexpr static qreal BOX_SIZE = SCENE_SIZE / 3;
    constexpr static qreal BOX_PADDING = 50;
    const static int PEN_WIDTH = 7;

    /*
     * Game logic
     */
    Board board;
    TTTMCTSPlayer ai;

    /*
     * Player selection
     */
    QGroupBox* playerSelect;
    QComboBox *player1Select, *player2Select;
    QLabel *player1, *player2;
    QPushButton* startGame;

    /*
     * Board drawing
     */
    QGraphicsScene* scene;
    QGraphicsView* view;
    QTimer* timer;
    QPen* pen;

public:
    GUI(QWidget* parent = 0);
    ~GUI();

private:
    void createPlayerSelect();
    void createBoard();
    void fillScene();

    QGraphicsItem* createCross(qreal size);
    QGraphicsItem* createCircle(qreal size);
    QGraphicsItem* createEmpty(qreal size);
    QGraphicsItem* createGrid(qreal size);

    void endGame();
    void playMove(int x, int y);

    bool isCurrentPlayerHuman() const;

private slots:
    void newGame();
    void boardClicked();
    void movePlayed();
};

#endif // CPP_MCTS_GUI_HPP
