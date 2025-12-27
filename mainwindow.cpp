#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>

#include "canvas/polylinecanvas.h"
#include "canvas/freespacecanvas.h"
#include "geometry/freespace.h"
#include "geometry/reachability.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- Central widget ---
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QHBoxLayout(central);

    // Left side: title + polyline canvas
    auto* leftLayout = new QVBoxLayout();

    auto* polylineTitle = new QLabel("Polylines P and Q");
    polylineTitle->setAlignment(Qt::AlignCenter);
    polylineTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

    polylineCanvas = new PolylineCanvas(this);

    leftLayout->addWidget(polylineTitle);
    leftLayout->addWidget(polylineCanvas, 1);

    mainLayout->addLayout(leftLayout, 1);

    // Right side: free space canvas + slider
    auto* rightLayout = new QVBoxLayout();

    auto* freeSpaceTitle = new QLabel("Free Space Diagram");
    freeSpaceTitle->setAlignment(Qt::AlignCenter);
    freeSpaceTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

    freeSpaceCanvas = new FreeSpaceCanvas(this);

    rightLayout->addWidget(freeSpaceTitle);
    rightLayout->addWidget(freeSpaceCanvas, 1);

    epsSlider = new QSlider(Qt::Horizontal);
    epsSlider->setMinimum(1);
    epsSlider->setMaximum(100);
    epsSlider->setValue(1);

    epsLabel = new QLabel(QString("Eps = %1").arg(epsSlider->value()));
    epsLabel->setAlignment(Qt::AlignCenter);

    rightLayout->addWidget(epsLabel);
    rightLayout->addWidget(epsSlider);

    mainLayout->addLayout(rightLayout, 1);

    // Polylines
    Polyline P;
    P.vertices = {
        QPointF(0, 0),
        QPointF(40, 0),
        QPointF(80, 30),
        QPointF(120, 60)
    };

    Polyline Q;
    Q.vertices = {
        QPointF(0, 50),
        QPointF(30, 80),
        QPointF(70, 20),
        QPointF(110, 50)
    };

    freeSpace = std::make_unique<FreeSpace>(P, Q, epsSlider->value());

    Frechet::Reachability reach(*freeSpace);
    reach.compute();

    polylineCanvas->setPolylines(P, Q);
    freeSpaceCanvas->setFreeSpace(freeSpace.get());

    // Signals & slots
    connect(epsSlider, &QSlider::valueChanged,
            this, &MainWindow::onEpsChanged);

    setWindowTitle("Frechet Distance Visualizer");
    resize(1100, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Slot
void MainWindow::onEpsChanged(int value)
{
    epsLabel->setText(QString("Eps = %1").arg(value));

    if (!freeSpace)
        return;

    // Update eps and recompute reachability
    freeSpace->setEps(value);
    freeSpace->computeReachability();
    freeSpaceCanvas->update();

    if (!freeSpace->pathComputed && freeSpace->isTopRightReachable()) {
        freeSpace->criticalPath = freeSpace->computeCriticalPath();
        freeSpace->pathComputed = true;
        freeSpace->criticalEps = value;
    }

    if (freeSpace->criticalPath.empty()) {
        qDebug() << "Critical path is empty!";
    } else {
        qDebug() << "Critical path coordinates:";
        for (const auto& pt : freeSpace->criticalPath) {
            qDebug() << "(" << pt.x() << "," << pt.y() << ")";
        }
    }
}
