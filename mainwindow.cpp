#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "canvas.h"
#include <QVBoxLayout>
#include "geometry/freespace.h"
#include "geometry/reachability.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    canvas = new Canvas(this);

    auto* layout = new QVBoxLayout(central);
    layout->addWidget(canvas);

    // Primer polilinija
    Polyline P;
    P.vertices = { QPointF(0,0), QPointF(2,0), QPointF(4,0) };
    Polyline Q;
    Q.vertices = { QPointF(1,1), QPointF(3,1), QPointF(5,1) };

    double eps = 2.0;

    FreeSpace fs(P, Q, eps);
    Frechet::Reachability reach(fs);
    reach.compute();

    // debug ispis
    qDebug() << "------REACHABLE------";
    auto& cells = fs.getCells();
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            const auto& c = cells[i][j];
            qDebug() << "Cell(" << i << "," << j << ")" <<
                "RB" << c.reachableBottom.toString() <<
                "RT" << c.reachableTop.toString() <<
                "RL" << c.reachableLeft.toString() <<
                "RR" << c.reachableRight.toString();
        }
    }

    qDebug() << "------FREE------";
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            const auto& c = cells[i][j];
            qDebug() << "Cell(" << i << "," << j << ")" <<
                "B" << c.bottom.toString() <<
                "T" << c.top.toString() <<
                "L" << c.left.toString() <<
                "R" << c.right.toString();
        }
    }

    canvas->setPolylines(P, Q);
}

MainWindow::~MainWindow()
{
    delete ui;
}
