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
            firstPlayer.insert(number);
            cell = std::unique_ptr<Cell>(new Cell(number, Way(number), FirstPlayerSimple));
        } else if (r > 5) {
            secondPlayer.insert(number);
            cell = std::unique_ptr<Cell>(new Cell(number, Way(number), SecondPlayerSimple));
        } else {
            cell = std::unique_ptr<Cell>(new Cell(number, Way(number), Empty));
        }
        ++number;
        connect(cell.get(), &Cell::highlighted, [&](size_t num) {
            cells[lightNumber].get()->offLighting();
            if (selectedCells.count(num) > 0) {
                qDebug() << cells[lightNumber].get()->getType();
                cells[num].get()->setType(cells[lightNumber].get()->getType());
                cells[lightNumber].get()->setType(Empty);
                if (state == First) {
                    firstPlayer.erase(lightNumber);
                    firstPlayer.insert(num);
                } else {
                    secondPlayer.erase(lightNumber);
                    secondPlayer.insert(num);
                }
                size_t del_cell = selectedCells[num];
                if (del_cell != NO_CUT) {
                    cells[del_cell].get()->setType(Empty);
                    if (state == First) {
                        firstPlayer.erase(del_cell);
                    } else {
                        secondPlayer.erase(del_cell);
                    }
                }

                state = (state == First) ? Second : First;
            }
            clearSelectedCells();
            lightNumber = num;
            if ((state == First && firstPlayer.count(lightNumber) > 0)
                    || (state == Second && secondPlayer.count(lightNumber) > 0)) {
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
        return {8, 20};
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
    clearSelectedCells();

    cells[lightNumber].get()->onLighting();
    Type t = cells[lightNumber].get()->getType();
    auto p = cells[lightNumber].get()->getWay();
    if (state == First) {
        if (t == FirstPlayerSimple) {
            auto it = next(ways[p.first].find(lightNumber));
            if (it != end(ways[p.first])
                    && cells[*it].get()->getType() != FirstPlayerSimple
                    && cells[*it].get()->getType() != FirstPlayerQween) {
                if (cells[*it].get()->getType() == Empty) {
                    selectedCells.insert({*it, NO_CUT});
                    cells[*it].get()->setSelected();
                } else {
                    auto d_it = it;
                    it = next(it);
                    if (it != end(ways[p.first]) && cells[*it].get()->getType() == Empty) {
                        selectedCells.insert({*it, *d_it});
                        cells[*it].get()->setSelected();
                    }
                }
            }
            if (p.second < 17) {
                it = next(ways[p.second].find(lightNumber));
                if (it != end(ways[p.second])
                        && cells[*it].get()->getType() != FirstPlayerSimple
                        && cells[*it].get()->getType() != FirstPlayerQween) {
                    if (cells[*it].get()->getType() == Empty) {
                        selectedCells.insert({*it, NO_CUT});
                        cells[*it].get()->setSelected();
                    } else {
                        auto d_it = it;
                        it = next(it);
                        if (it != end(ways[p.second]) && cells[*it].get()->getType() == Empty) {
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
        qDebug() << p;
        if (t == SecondPlayerSimple) {
            qDebug() << "pp";
            auto it = ways[p.first].find(lightNumber);
            if (it != begin(ways[p.first])) {
                it = prev(it);
                if (cells[*it].get()->getType() != SecondPlayerSimple
                        && cells[*it].get()->getType() != SecondPlayerQween) {

                    if (cells[*it].get()->getType() == Empty) {
                        selectedCells.insert({*it, NO_CUT});
                        cells[*it].get()->setSelected();
                    } else {
                        if (it != begin(ways[p.first])) {
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

            if (p.second < 17) {
                it = ways[p.second].find(lightNumber);
                if (it != begin(ways[p.second])) {
                    it = prev(it);
                    if (cells[*it].get()->getType() != SecondPlayerSimple
                            && cells[*it].get()->getType() != SecondPlayerQween) {

                        if (cells[*it].get()->getType() == Empty) {
                            selectedCells.insert({*it, NO_CUT});
                            cells[*it].get()->setSelected();
                        } else {
                            if (it != begin(ways[p.second])) {
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

void Board::clearSelectedCells()
{
    for (auto i : selectedCells) {
        cells[i.first].get()->offLighting();
    }
    selectedCells.clear();
}
