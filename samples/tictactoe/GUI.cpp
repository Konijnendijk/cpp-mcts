#include <iostream>

#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QPen>
#include <QTimer>
#include <QVBoxLayout>

#include "GUI.hpp"
#include "TTTAction.hpp"

GUI::GUI(QWidget* parent)
    : QWidget(parent)
    , board(), ai()
{
    createPlayerSelect();
    createBoard();

    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget(playerSelect);
    layout->addWidget(view);

    setLayout(layout);

    connect(startGame, SIGNAL(clicked(bool)), this, SLOT(newGame()));
}

void GUI::createPlayerSelect()
{
    playerSelect = new QGroupBox(tr("Players"));

    player1 = new QLabel(tr("Cross:"));
    player2 = new QLabel(tr("Circle:"));

    player1Select = new QComboBox();
    player2Select = new QComboBox();

    player1Select->addItem(tr("Human"));
    player1Select->addItem(tr("MCTS AI"));

    player2Select->addItem(tr("Human"));
    player2Select->addItem(tr("MCTS AI"));

    startGame = new QPushButton(tr("Start Game"));

    QHBoxLayout* selectLayout = new QHBoxLayout();

    selectLayout->addWidget(player1);
    selectLayout->addWidget(player1Select);
    selectLayout->addWidget(player2);
    selectLayout->addWidget(player2Select);
    selectLayout->addWidget(startGame);

    playerSelect->setLayout(selectLayout);
}

void GUI::createBoard()
{
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, SCENE_SIZE, SCENE_SIZE);

    pen = new QPen();
    pen->setWidth(PEN_WIDTH);

    fillScene();

    view = new QGraphicsView(scene);

    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(movePlayed()));
}

void GUI::fillScene()
{
    scene->clear();

    scene->addItem(createGrid(scene->width()));

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            QGraphicsItem* empty = createEmpty(BOX_SIZE);
            empty->setPos(x * BOX_SIZE, y * BOX_SIZE);
            scene->addItem(empty);
        }
    }
}

QGraphicsItem* GUI::createCross(qreal size)
{
    QGraphicsItemGroup* group = new QGraphicsItemGroup();
    QGraphicsLineItem* line = new QGraphicsLineItem(BOX_PADDING, BOX_PADDING, size - BOX_PADDING, size - BOX_PADDING);
    line->setPen(*pen);
    group->addToGroup(line);
    line = new QGraphicsLineItem(size - BOX_PADDING, BOX_PADDING, BOX_PADDING, size - BOX_PADDING);
    line->setPen(*pen);
    group->addToGroup(line);
    group->setData(0, CROSS);
    return group;
}

QGraphicsItem* GUI::createCircle(qreal size)
{
    QGraphicsEllipseItem* item
        = new QGraphicsEllipseItem(BOX_PADDING, BOX_PADDING, size - BOX_PADDING * 2, size - BOX_PADDING * 2);
    item->setPen(*pen);
    item->setData(0, CIRCLE);
    return item;
}

QGraphicsItem* GUI::createEmpty(qreal size)
{
    QGraphicsWidget* item = new QGraphicsWidget();
    item->resize(size, size);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setData(0, NONE);
    return item;
}

QGraphicsItem* GUI::createGrid(qreal size)
{
    QGraphicsItemGroup* group = new QGraphicsItemGroup();

    qreal step = size / 3;
    for (int i = 1; i < 3; i++) {
        QGraphicsLineItem* line = new QGraphicsLineItem(i * step, 0, i * step, size);
        line->setPen(*pen);
        group->addToGroup(line);
        line = new QGraphicsLineItem(0, i * step, size, i * step);
        line->setPen(*pen);
        group->addToGroup(line);
    }

    return group;
}

void GUI::endGame()
{
    player1Select->setDisabled(false);
    player2Select->setDisabled(false);
    startGame->setDisabled(false);

    disconnect(scene, 0, this, 0);
}

void GUI::playMove(int x, int y)
{
    QGraphicsItem* item = scene->itemAt(x * BOX_SIZE, y * BOX_SIZE, view->transform());
    scene->removeItem(item);
    //	delete item;

    item = board.getCurrentPlayer() == CROSS ? createCross(BOX_SIZE) : createCircle(BOX_SIZE);
    item->setPos(x * BOX_SIZE, y * BOX_SIZE);
    scene->addItem(item);

    item->update(item->boundingRect());

    board.play(x, y);

    // Return control back to event loop for redrawing
    timer->start();
}

bool GUI::isCurrentPlayerHuman()
{
    Player current = board.getCurrentPlayer();
    int index1 = player1Select->currentIndex();
    int index2 = player2Select->currentIndex();
    return (current == CROSS && index1 == 0) || (current == CIRCLE && index2 == 0);
}

void GUI::movePlayed()
{
    if (board.won() != NONE || board.getTurns() >= 9) {
        endGame();
        return;
    }

    if (!isCurrentPlayerHuman()) {
        auto action = ai.calculateAction(board);
        playMove(action.getX(), action.getY());
    }
}

void GUI::newGame()
{
    player1Select->setDisabled(true);
    player2Select->setDisabled(true);
    startGame->setDisabled(true);

    connect(scene, SIGNAL(selectionChanged()), this, SLOT(boardClicked()));

    fillScene();

    board = Board();

    // Return control back to event loop for redrawing
    timer->start();
}

void GUI::boardClicked()
{
    if (isCurrentPlayerHuman()) {
        auto selected = scene->selectedItems();
        if (selected.size() > 0) {
            QGraphicsItem* item = selected[0];
            if (item->data(0) == NONE) {
                playMove(item->x() / BOX_SIZE, item->y() / BOX_SIZE);
            }
        }
    }
}

GUI::~GUI() {}
