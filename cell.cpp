#include "cell.h"

Cell::Cell(size_t number, std::pair<size_t, size_t> way, Type t, QLabel *parent)
    : QLabel(parent)
    , type(t)
    , number(number)
    , way(way)
{
    selected = QPalette(Qt::cyan);
    highlight = QPalette(Qt::darkGreen);
    empty = QPalette(Qt::darkYellow);

    setFixedSize(50, 50);
    setAutoFillBackground(true);
    setPalette(empty);
}

void Cell::offLighting()
{
    setPalette(empty);
}

void Cell::onLighting()
{
    setPalette(highlight);
}

void Cell::setSelected()
{
    setPalette(selected);
}

size_t Cell::getNumber() const
{
    return number;
}

void Cell::setNumber(const size_t &value)
{
    number = value;
}

std::pair<size_t, size_t> Cell::getWay() const
{
    return way;
}

Type Cell::getType() const
{
    return type;
}

void Cell::setType(const Type &value)
{
    type = value;
    repaint();
}


void Cell::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    switch (type) {
    case FirstPlayerSimple:
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::black));
        p.drawEllipse(2, 2, 45, 45);
        break;
    case SecondPlayerSimple:
        p.setPen(QPen(Qt::white));
        p.setBrush(QBrush(Qt::white));
        p.drawEllipse(2, 2, 45, 45);
        break;
    case FirstPlayerQueen:
        p.setPen(QPen(Qt::black));
        p.setBrush(QBrush(Qt::black));
        p.drawEllipse(2, 2, 45, 45);
        p.setPen(QPen(Qt::white, 4));
        p.drawEllipse(10, 10, 30, 30);
        break;
    case SecondPlayerQueen:
        p.setPen(QPen(Qt::white));
        p.setBrush(QBrush(Qt::white));
        p.drawEllipse(2, 2, 45, 45);
        p.setPen(QPen(Qt::black, 4));
        p.drawEllipse(10, 10, 30, 30);
        break;
    default:
        break;
    }
}

void Cell::mousePressEvent(QMouseEvent *event)
{
    emit highlighted(number);
}
