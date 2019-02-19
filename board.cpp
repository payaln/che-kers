#include "board.h"

Board::Board(QWidget *parent) : QWidget(parent)
{

    setFixedSize(500, 500);
    QGridLayout* grid = new QGridLayout;
    grid->setMargin(1);
    grid->setSpacing(0);
    grid->setHorizontalSpacing(1);
    grid->setVerticalSpacing(1);

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
        connect(cell.get(), &Cell::highlighted, this, &Board::onClickCell);

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

void Board::calckStepsQueen(size_t w1, size_t w2)
{
    auto it = next(ways[w1].find(lightNumber));
    while (it != end(ways[w1]) && players[state].count(*it) == 0) {
        if (cells[*it].get()->getType() == Empty) {
            selectedCells.insert({*it, NO_CUT});
            cells[*it].get()->setSelected();
        } else {
            break;
        }
        it = next(it);
    }
    it = ways[w1].find(lightNumber);
    do {
        if (it == begin(ways[w1])) {
            break;
        }
        it = prev(it);
        if (cells[*it].get()->getType() == Empty) {
            selectedCells.insert({*it, NO_CUT});
            cells[*it].get()->setSelected();
        } else {
            break;
        }
    } while (it != begin(ways[w1]) && players[state].count(*it) == 0);
    if (w2 != NO_WAY) {
        it = next(ways[w2].find(lightNumber));
        while (it != end(ways[w2]) && players[state].count(*it) == 0) {
            if (cells[*it].get()->getType() == Empty) {
                selectedCells.insert({*it, NO_CUT});
                cells[*it].get()->setSelected();
            } else {
                break;
            }
            it = next(it);
        }
        it = ways[w2].find(lightNumber);
        do {
            if (it == begin(ways[w2])) {
                break;
            }
            it = prev(it);
            if (cells[*it].get()->getType() == Empty) {
                selectedCells.insert({*it, NO_CUT});
                cells[*it].get()->setSelected();
            } else {
                break;
            }
        } while (it != begin(ways[w2]) && players[state].count(*it) == 0);
    }
}

void Board::calckSteps()
{
    calckFight();

    cells[lightNumber].get()->onLighting();
    Type t = cells[lightNumber].get()->getType();
    auto [w1, w2] = cells[lightNumber].get()->getWay();
    if (isFight && (checkWay(w1, lightNumber) || checkWay(w2, lightNumber))) {
        lightFigtingCells(t);
    } else if (!isFight) {
        if (t == FirstPlayerQueen || t == SecondPlayerQueen) {
            calckStepsQueen(w1, w2);
        } else {
            if (t == FirstPlayerSimple) {
                auto it = next(ways[w1].find(lightNumber));
                if (it != end(ways[w1]) && players[First].count(*it) == 0 && cells[*it].get()->getType() == Empty) {
                    selectedCells.insert({*it, NO_CUT});
                    cells[*it].get()->setSelected();
                }
                if (w2 != NO_WAY) {
                    it = next(ways[w2].find(lightNumber));
                    if (it != end(ways[w2]) && players[First].count(*it) == 0 && cells[*it].get()->getType() == Empty) {
                        selectedCells.insert({*it, NO_CUT});
                        cells[*it].get()->setSelected();
                    }
                }
            }
            if (t == SecondPlayerSimple) {
                auto it = ways[w1].find(lightNumber);
                if (it != begin(ways[w1])) {
                    it = prev(it);
                    if (players[Second].count(*it) == 0 && cells[*it].get()->getType() == Empty) {
                        selectedCells.insert({*it, NO_CUT});
                        cells[*it].get()->setSelected();
                    }
                }
                if (w2 != NO_WAY) {
                    it = ways[w2].find(lightNumber);
                    if (it != begin(ways[w2])) {
                        it = prev(it);
                        if (players[Second].count(*it) == 0 && cells[*it].get()->getType() == Empty) {
                            selectedCells.insert({*it, NO_CUT});
                            cells[*it].get()->setSelected();
                        }
                    }
                }
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
    isFight = false;
    for (const auto& i : players[state]) {
        auto [way1, way2] = cells[i].get()->getWay();
        if (checkWay(way1, i) || checkWay(way2, i)) {
            isFight = true;
            break;
        }
    }
}

bool Board::checkWayPr(size_t way, size_t cell)
{
    if (way == NO_WAY) {
        return false;
    }
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
    if (way == NO_WAY) {
        return false;
    }
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
    Type t = cells[cell].get()->getType();
    if (t == FirstPlayerQueen || t == SecondPlayerQueen) {
        if (way != NO_WAY) {
            auto it = next(ways[way].find(cell));
            while (it != end(ways[way]) && cells[*it].get()->getType() == Empty) {
                it = next(it);
            }
            if (it != end(ways[way]) && players[state].count(*it) == 0) {
                it = next(it);
                if (it != end(ways[way]) && cells[*it].get()->getType() == Empty) {
                    return true;
                }
            }
            it = ways[way].find(cell);
            while (it != begin(ways[way])) {
                it = prev(it);
                if (cells[*it].get()->getType() != Empty) {
                    break;
                }
            }
            if (it != begin(ways[way]) && players[state].count(*it) == 0) {
                it = prev(it);
                if (cells[*it].get()->getType() == Empty) {
                    return true;
                }
            }
        }
        return false;
    }
    return checkWayPr(way, cell) || checkWayNx(way, cell);
}

void Board::lightFigtingCellsSimple()
{
    auto [w1, w2] = cells[lightNumber].get()->getWay();
    auto it1 = ways[w1].find(lightNumber);
    if (checkWayNx(w1, lightNumber) && deletedCells.count(*next(it1)) == 0) {
        selectedCells.insert({*next(it1, 2), *next(it1)});
        cells[*next(it1, 2)].get()->setSelected();
    }
    if (checkWayPr(w1, lightNumber) && deletedCells.count(*prev(it1)) == 0) {
        selectedCells.insert({*prev(it1, 2), *prev(it1)});
        cells[*prev(it1, 2)].get()->setSelected();
    }
    if (checkWayNx(w2, lightNumber)) {
        auto it2 = ways[w2].find(lightNumber);
        if (deletedCells.count(*next(it2)) == 0) {
            selectedCells.insert({*next(it2, 2), *next(it2)});
            cells[*next(it2, 2)].get()->setSelected();
        }
    }
    if (checkWayPr(w2, lightNumber)) {
        auto it2 = ways[w2].find(lightNumber);
        if (deletedCells.count(*prev(it2)) == 0) {
            selectedCells.insert({*prev(it2, 2), *prev(it2)});
            cells[*prev(it2, 2)].get()->setSelected();
        }
    }
}

void Board::lightFigtingCellsQueen()
{
    auto [w1, w2] = cells[lightNumber].get()->getWay();
    auto it = next(ways[w1].find(lightNumber));
    while (it != end(ways[w1]) && cells[*it].get()->getType() == Empty) {
        it = next(it);
    }
    if (it != end(ways[w1]) && players[state].count(*it) == 0 && deletedCells.count(*it) == 0) {
        auto del_it = it;
        it = next(it);
        while (it != end(ways[w1]) && cells[*it].get()->getType() == Empty) {
            selectedCells.insert({*it, *del_it});
            cells[*it].get()->setSelected();
            it = next(it);
        }
    }
    it = ways[w1].find(lightNumber);
    while (it != begin(ways[w1])) {
        it = prev(it);
        if (cells[*it].get()->getType() != Empty) {
            break;
        }
    }
    if (it != begin(ways[w1]) && players[state].count(*it) == 0 && deletedCells.count(*it) == 0) {
        auto del_it = it;
        it = prev(it);
        while (cells[*it].get()->getType() == Empty) {
            selectedCells.insert({*it, *del_it});
            cells[*it].get()->setSelected();
            if (it == begin(ways[w1])) {
                break;
            }
            it = prev(it);
        }
    }
    if (w2 != NO_WAY) {
        it = next(ways[w2].find(lightNumber));
        while (it != end(ways[w2]) && cells[*it].get()->getType() == Empty) {
            it = next(it);
        }
        if (it != end(ways[w2]) && players[state].count(*it) == 0 && deletedCells.count(*it) == 0) {
            auto del_it = it;
            it = next(it);
            while (it != end(ways[w2]) && cells[*it].get()->getType() == Empty) {
                selectedCells.insert({*it, *del_it});
                cells[*it].get()->setSelected();
                it = next(it);
            }
        }
        it = ways[w2].find(lightNumber);
        while (it != begin(ways[w2])) {
            it = prev(it);
            if (cells[*it].get()->getType() != Empty) {
                break;
            }
        }
        if (it != begin(ways[w2]) && players[state].count(*it) == 0 && deletedCells.count(*it) == 0) {
            auto del_it = it;
            it = prev(it);
            while (cells[*it].get()->getType() == Empty) {
                selectedCells.insert({*it, *del_it});
                cells[*it].get()->setSelected();
                if (it == begin(ways[w2])) {
                    break;
                }
                it = prev(it);
            }
        }
    }
}

void Board::lightFigtingCells(Type t)
{
    if (t == FirstPlayerQueen || t == SecondPlayerQueen) {
        lightFigtingCellsQueen();
    } else {
        lightFigtingCellsSimple();
    }
}

void Board::identifyQueen(size_t cell)
{
    if (players[First].count(cell) > 0 && cell > 45) {
        cells[cell].get()->setType(FirstPlayerQueen);
    }
    if (players[Second].count(cell) > 0 && cell < 5) {
        cells[cell].get()->setType(SecondPlayerQueen);
    }
}

Player Board::nextState()
{
    if (state == First) {
        return Second;
    } else {
        return First;
    }
}

void Board::onClickCell(size_t num)
{
    cells[lightNumber].get()->offLighting();
    if (selectedCells.count(num) > 0) {
        cells[num].get()->setType(cells[lightNumber].get()->getType());
        cells[lightNumber].get()->setType(Empty);
        players[state].erase(lightNumber);
        players[state].insert(num);
        deletedCells.insert(selectedCells[num]);

        auto [w1, w2] = cells[num].get()->getWay();
        if ((checkWay(w1, num) || checkWay(w2, num)) && selectedCells[num] != NO_CUT) {
            clearSelectedCells();
            lightNumber = num;
            lightFigtingCells(cells[lightNumber].get()->getType());
            if (selectedCells.size() > 0) {
                cells[lightNumber].get()->onLighting();
                return;
            }
        }

        for(auto del_cell : deletedCells) {
            if (del_cell != NO_CUT) {
                cells[del_cell].get()->setType(Empty);
                players[nextState()].erase(del_cell);
            }
        }
        if (players[nextState()].empty()) {
            QMessageBox::information(nullptr, "Победитель", QString("Победил игрок №%1!").arg(state + 1));
        }
        deletedCells.clear();
        identifyQueen(num);
        state = (state == First) ? Second : First;
    }
    clearSelectedCells();
    lightNumber = num;
    if (players[state].count(lightNumber) > 0) {
        calckSteps();
    }
}
