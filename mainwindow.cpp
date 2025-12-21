#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "canvas.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    canvas = new Canvas(this);

    auto* layout = new QVBoxLayout(central);
    layout->addWidget(canvas);

    Polyline P({{50,50},{150,80},{300,60}});
    Polyline Q({{60,200},{180,220},{320,210}});

    canvas->setPolylines(P, Q);

}

MainWindow::~MainWindow()
{
    delete ui;
}
