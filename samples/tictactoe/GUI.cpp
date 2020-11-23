#include <QGraphicsEllipseItem>
#include <QGraphicsWidget>
#include <QHBoxLayout>
#include <QPen>
#include <QTimer>

#include "GUI.hpp"
#include "TTTAction.hpp"

Q_DECLARE_METATYPE(Player)

GUI::GUI(QWidget* parent)
    : QWidget(parent)
    , playerSelect()
    , player1Select()
    , player2Select()
    , player1()
    , player2()
    , startGame()
    , scene()
    , view()
    , timer()
    , pen()
{
    createPlayerSelect();
    createBoard();

    auto layout = new QVBoxLayout();

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

    auto selectLayout = new QHBoxLayout();

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
    auto group = new QGraphicsItemGroup();
    auto line = new QGraphicsLineItem(BOX_PADDING, BOX_PADDING, size - BOX_PADDING, size - BOX_PADDING);
    line->setPen(*pen);
    group->addToGroup(line);
    line = new QGraphicsLineItem(size - BOX_PADDING, BOX_PADDING, BOX_PADDING, size - BOX_PADDING);
    line->setPen(*pen);
    group->addToGroup(line);
    group->setData(0, QVariant::fromValue(Player::CROSS));
    return group;
}

QGraphicsItem* GUI::createCircle(qreal size)
{
    auto item
        = new QGraphicsEllipseItem(BOX_PADDING, BOX_PADDING, size - BOX_PADDING * 2, size - BOX_PADDING * 2);
    item->setPen(*pen);
    item->setData(0, QVariant::fromValue(Player::CIRCLE));
    return item;
}

QGraphicsItem* GUI::createGrid(qreal size)
{
    auto group = new QGraphicsItemGroup();

    qreal step = size / 3;
    for (int i = 1; i < 3; i++) {
        auto line = new QGraphicsLineItem(i * step, 0, i * step, size);
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

    disconnect(scene, nullptr, this, nullptr);
}

void GUI::playMove(int x, int y)
{
    QGraphicsItem* item = scene->itemAt(x * BOX_SIZE, y * BOX_SIZE, view->transform());
    scene->removeItem(item);
    //	delete item;

    item = board.getCurrentPlayer() == Player::CROSS ? createCross(BOX_SIZE) : createCircle(BOX_SIZE);
    item->setPos(x * BOX_SIZE, y * BOX_SIZE);
    scene->addItem(item);

    item->update(item->boundingRect());

    board.play(x, y);

    // Return control back to event loop for redrawing
    timer->start();
}

bool GUI::isCurrentPlayerHuman() const
{
    Player current = board.getCurrentPlayer();
    int index1 = player1Select->currentIndex();
    int index2 = player2Select->currentIndex();
    return (current == Player::CROSS && index1 == 0) || (current == Player::CIRCLE && index2 == 0);
}

void GUI::movePlayed()
{
    if (board.won() != Player::NONE || board.getTurns() >= 9) {
        endGame();
        return;
    }

    if (!isCurrentPlayerHuman()) {
        auto action = TTTMCTSPlayer::calculateAction(board);
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
        if (!selected.empty()) {
            QGraphicsItem* item = selected[0];
            if (item->data(0) == QVariant::fromValue(Player::NONE)) {
                playMove((int)(item->x() / BOX_SIZE), (int)(item->y() / BOX_SIZE));
            }
        }
    }
}

QGraphicsItem* GUI::createEmpty(qreal size)
{
    auto item = new QGraphicsWidget();
    item->resize(size, size);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setData(0, QVariant::fromValue(Player::NONE));
    return item;
}
