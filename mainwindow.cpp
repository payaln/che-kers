#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "board.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Board* b = new Board();

    ui->centralWidget->layout()->addWidget(b);
}

MainWindow::~MainWindow()
{
    delete ui;
}
