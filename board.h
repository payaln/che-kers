#ifndef BOARD_H
#define BOARD_H

#include <QGridLayout>
#include <QMessageBox>
#include <QWidget>
#include <array>
#include <memory>
#include <set>
#include <map>

#include <cell.h>

#define BOARD_SIZE 10

#define NO_CUT 100

#define NO_WAY 20

enum Player {
    First, // 0
    Second // 1
};

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr);

private:
    std::array<std::unique_ptr<Cell>, 50> cells;
    size_t lightNumber = 0;
    Player state = First;
    std::map<Player, std::set<size_t>> players;
    std::map<size_t, size_t> selectedCells;
    std::set<size_t> deletedCells;
    bool isFight = false;

    std::vector<std::set<size_t>> ways = {
        {0, 5},
        {0, 6, 11, 17, 22, 28, 33, 39, 44},
        {1, 6, 10, 15},
        {1, 7, 12, 18, 23, 29, 34},
        {2, 7, 11, 16, 20, 25},
        {2, 8, 13, 19, 24},
        {3, 8, 12, 17, 21, 26, 30, 35},
        {3, 9, 14},
        {4, 9, 13, 18, 22, 27, 31, 36, 40, 45}, //8
        {5, 10, 16, 21, 27, 32, 38, 43, 49},
        {14, 19, 23, 28, 32, 37, 41, 46},
        {15, 20, 26, 31, 37, 42, 48},
        {24, 29, 33, 38, 42, 47},
        {25, 30, 36, 41, 47},
        {34, 39, 43, 48},
        {35, 40, 46},
        {44, 49}
    };

    std::pair<size_t, size_t> Way(size_t cell_number);
    void calckSteps();
    void clearSelectedCells();
    void calckFight();
    bool checkWayPr(size_t way, size_t cell);
    bool checkWayNx(size_t way, size_t cell);
    bool checkWay(size_t way, size_t cell);
    void lightFigtingCells(Type t);
    void lightFigtingCellsQueen();
    void lightFigtingCellsSimple();
    void identifyQueen(size_t cell);
    void calckStepsQueen(size_t w1, size_t w2);
    Player nextState();

public slots:
    void onClickCell(size_t num);
};

#endif // BOARD_H
