#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include <QLabel>

enum Type {
    Empty,
    FirstPlayerSimple,
    SecondPlayerSimple,
    FirstPlayerQween,
    SecondPlayerQween
};

class Cell : public QLabel
{
    Q_OBJECT
public:
    explicit Cell(size_t number, std::pair<size_t, size_t> way, Type t = Empty, QLabel *parent = nullptr);
    void offLighting();
    void onLighting();
    void setSelected();

    size_t getNumber() const;
    void setNumber(const size_t &value);

    std::pair<size_t, size_t> getWay() const;

    Type getType() const;
    void setType(const Type &value);

private:
    Type type;
    size_t number;
    std::pair<size_t, size_t> way;

signals:
    void highlighted(size_t number);

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // CELL_H
