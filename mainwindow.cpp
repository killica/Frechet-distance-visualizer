#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "canvas/polylinecanvas.h"
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

    // Horizontalni layout: levo polilinije, desno free space
    auto* layout = new QHBoxLayout(central);

    polylineCanvas = new PolylineCanvas(this);
    layout->addWidget(polylineCanvas, 1);

    freeSpaceCanvas = new FreeSpaceCanvas(this);
    layout->addWidget(freeSpaceCanvas, 1);

    // Primer polilinija
    Polyline P;
    P.vertices = { QPointF(0,0), QPointF(200,0), QPointF(400,0) };
    Polyline Q;
    Q.vertices = { QPointF(100,100), QPointF(300,100), QPointF(500,100) };

    double eps = 200.0;

    freeSpace = std::make_unique<FreeSpace>(P, Q, eps);
    Frechet::Reachability reach(*freeSpace);
    reach.compute();

    // debug ispis
    qDebug() << "------REACHABLE------";
    auto& cells = freeSpace->getCells();
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

    polylineCanvas->setPolylines(P, Q);
    freeSpaceCanvas->setFreeSpace(freeSpace.get());

    setWindowTitle("Frechet Distance Visualizer");
    resize(1100, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}
