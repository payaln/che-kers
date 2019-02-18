#include "board.h"
#include "cell.h"
#include <QGridLayout>
#include <QDebug>

Board::Board(QWidget *parent) : QWidget(parent)
{

    QGridLayout* grid = new QGridLayout;
    grid->setMargin(1);
    grid->setSpacing(0);
    grid->setHorizontalSpacing(1);
    grid->setVerticalSpacing(1);
//    grid->setSizeConstraint(QLayout::SetMinimumSize);
//    Cell* c = new Cell();
//    grid->addWidget(c, 0, 0, 1, 0);
//    Cell* c1 = new Cell();
//    grid->addWidget(c1, 0, 1, 0, 0);
//    Cell* c2 = new Cell();
//    grid->addWidget(c2, 0, 2, 0, 4);

    size_t r = 0;
    size_t number = 0;
    size_t col = 1;
    for (auto& cell : cells) {
        if (r < 4) {
            players[First].insert(number);
            cell = std::unique_ptr<Cell>(new Cell(number, Way(number), FirstPlayerSimple));
        } else if (r > 5) {
            players[Second].insert(number);
            cell = std::unique_ptr<Cell>(new Cell(number, Way(number), SecondPlayerSimple));
        } else {
            cell = std::unique_ptr<Cell>(new Cell(number, Way(number), Empty));
        }
        ++number;
        connect(cell.get(), &Cell::highlighted, [&](size_t num) {
            cells[lightNumber].get()->offLighting();
            if (selectedCells.count(num) > 0) {
                cells[num].get()->setType(cells[lightNumber].get()->getType());
                cells[lightNumber].get()->setType(Empty);
                players[state].erase(lightNumber);
                players[state].insert(num);
                size_t del_cell = selectedCells[num];
                if (del_cell != NO_CUT) {
                    cells[del_cell].get()->setType(Empty);
                    players[state].erase(del_cell);
                }

                state = (state == First) ? Second : First;
            }
            clearSelectedCells();
            lightNumber = num;
            if (players[state].count(lightNumber) > 0) {
                calckSteps();
            }
        });

        grid->addWidget(cell.get(), r, col, 1, 1);
        col += 2;
        if (col >= 10) {
            col = r % 2;
            ++r;
        }
    }
    state = First;
    setLayout(grid);

}

std::pair<size_t, size_t> Board::Way(size_t cell_number)
{
    if (cell_number == 4 || cell_number == 45) {
        return {8, NO_WAY};
    }
    size_t f = 20;
    size_t counter = 0;
    for (const auto& way : ways) {
        if (way.count(cell_number) > 0) {
            if (f < 20) {
                return {f, counter};
            } else {
                f = counter;
            }
        }
        ++counter;
    }
}

void Board::calckSteps()
{
    calckFight();

    cells[lightNumber].get()->onLighting();
    Type t = cells[lightNumber].get()->getType();
    auto [w1, w2] = cells[lightNumber].get()->getWay();

    if ((isFight && (checkWay(w1, lightNumber) || checkWay(w2, lightNumber))) || !isFight) {
        if (state == First) {
            if (t == FirstPlayerSimple) {
                auto it = next(ways[w1].find(lightNumber));
                if (it != end(ways[w1])
                        && cells[*it].get()->getType() != FirstPlayerSimple
                        && cells[*it].get()->getType() != FirstPlayerQween) {
                    if (cells[*it].get()->getType() == Empty) {
                        if (!(checkWay(w1, lightNumber) || checkWay(w2, lightNumber))) {
                            selectedCells.insert({*it, NO_CUT});
                            cells[*it].get()->setSelected();
                        }
                    } else {
                        auto d_it = it;
                        it = next(it);
                        if (it != end(ways[w1]) && cells[*it].get()->getType() == Empty) {
                            selectedCells.insert({*it, *d_it});
                            cells[*it].get()->setSelected();
                        }
                    }
                }
                if (w2 != NO_WAY) {
                    it = next(ways[w2].find(lightNumber));
                    if (it != end(ways[w2])
                            && cells[*it].get()->getType() != FirstPlayerSimple
                            && cells[*it].get()->getType() != FirstPlayerQween) {
                        if (cells[*it].get()->getType() == Empty) {
                            if (!(checkWay(w1, lightNumber) || checkWay(w2, lightNumber))) {
                                selectedCells.insert({*it, NO_CUT});
                                cells[*it].get()->setSelected();
                            }
                        } else {
                            auto d_it = it;
                            it = next(it);
                            if (it != end(ways[w2]) && cells[*it].get()->getType() == Empty) {
                                selectedCells.insert({*it, *d_it});
                                cells[*it].get()->setSelected();
                            }
                        }
                    }
                }

            } else {
    //            todo: дамка
            }

        } else {
            if (t == SecondPlayerSimple) {
                auto it = ways[w1].find(lightNumber);
                if (it != begin(ways[w1])) {
                    it = prev(it);
                    if (cells[*it].get()->getType() != SecondPlayerSimple
                            && cells[*it].get()->getType() != SecondPlayerQween) {
                        if (cells[*it].get()->getType() == Empty) {
                            if (!(checkWay(w1, lightNumber) || checkWay(w2, lightNumber))) {
                                selectedCells.insert({*it, NO_CUT});
                                cells[*it].get()->setSelected();
                            }
                        } else {
                            if (it != begin(ways[w1])) {
                                auto d_it = it;
                                it = prev(it);
                                if (cells[*it].get()->getType() == Empty) {
                                    selectedCells.insert({*it, *d_it});
                                    cells[*it].get()->setSelected();
                                }
                            }
                        }
                    }
                }

                if (w2 != NO_WAY) {
                    it = ways[w2].find(lightNumber);
                    if (it != begin(ways[w2])) {
                        it = prev(it);
                        if (cells[*it].get()->getType() != SecondPlayerSimple
                                && cells[*it].get()->getType() != SecondPlayerQween) {

                            if (cells[*it].get()->getType() == Empty) {
                                if (!(checkWay(w1, lightNumber) || checkWay(w2, lightNumber))) {
                                    selectedCells.insert({*it, NO_CUT});
                                    cells[*it].get()->setSelected();
                                }
                            } else {
                                if (it != begin(ways[w2])) {
                                    auto d_it = it;
                                    it = prev(it);
                                    if (cells[*it].get()->getType() == Empty) {
                                        selectedCells.insert({*it, *d_it});
                                        cells[*it].get()->setSelected();
                                    }
                                }
                            }
                        }
                    }
                }

            } else {
    //            todo: дамка
            }
        }
    }
}

void Board::clearSelectedCells()
{
    for (auto i : selectedCells) {
        cells[i.first].get()->offLighting();
    }
    selectedCells.clear();
}

void Board::calckFight()
{
    for (const auto& i : players[state]) {
        if (cells[i].get()->getType() == FirstPlayerSimple ||
            cells[i].get()->getType() == SecondPlayerSimple) {
            auto [way1, way2] = cells[i].get()->getWay();
            if (checkWay(way1, i) || checkWay(way2, i)) {
                isFight = true;
                break;
            }
        } else {
            // дамка
        }
    }
}

bool Board::checkWayPr(size_t way, size_t cell)
{
    auto it = ways[way].find(cell);
    if (it != begin(ways[way])) {
        it = prev(it);
        if (players[nextState()].count(*it) > 0) {
            if (it != begin(ways[way]) && cells[*prev(it)].get()->getType() == Empty) {
                return true;
            }
        }
    }
    return false;
}

bool Board::checkWayNx(size_t way, size_t cell)
{
    auto it = next(ways[way].find(cell));
    if (it != end(ways[way])) {
        if (players[nextState()].count(*it) > 0) {
            it = next(it);
            if (it != end(ways[way]) && cells[*it].get()->getType() == Empty) {
                return true;
            }
        }
    }
    return false;
}

bool Board::checkWay(size_t way, size_t cell)
{
    return checkWayPr(way, cell) || checkWayNx(way, cell);
}

Player Board::nextState()
{
    if (state == First) {
        return Second;
    } else {
        return First;
    }
}

