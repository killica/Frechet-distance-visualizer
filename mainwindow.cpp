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

    auto* mainLayout = new QHBoxLayout(central);

    // --- Levi canvas za polilinije ---
    polylineCanvas = new PolylineCanvas(this);
    mainLayout->addWidget(polylineCanvas, 1);

    // --- Desni layout za free space + slider ---
    auto* rightLayout = new QVBoxLayout();
    freeSpaceCanvas = new FreeSpaceCanvas(this);
    rightLayout->addWidget(freeSpaceCanvas, 1); // rastegne se

    epsSlider = new QSlider(Qt::Horizontal);
    epsSlider->setMinimum(1);
    epsSlider->setMaximum(100);
    epsSlider->setValue(69);
    epsLabel = new QLabel(QString("Eps = %1").arg(epsSlider->value()));

    rightLayout->addWidget(epsLabel);
    rightLayout->addWidget(epsSlider);

    mainLayout->addLayout(rightLayout, 1);

    // --- Polilinije ---
    Polyline P;
    P.vertices = { QPointF(0,0), QPointF(40,0), QPointF(80,30), QPointF(120,60) };
    Polyline Q;
    Q.vertices = { QPointF(0,50), QPointF(30,80), QPointF(70,20), QPointF(110,50) };

    freeSpace = std::make_unique<FreeSpace>(P, Q, epsSlider->value());
    Frechet::Reachability reach(*freeSpace);
    reach.compute();

    polylineCanvas->setPolylines(P, Q);
    freeSpaceCanvas->setFreeSpace(freeSpace.get());

    // --- Signal i slot ---
    connect(epsSlider, &QSlider::valueChanged, this, &MainWindow::onEpsChanged);

    setWindowTitle("Frechet Distance Visualizer");
    resize(1100, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- Slot ---
void MainWindow::onEpsChanged(int value)
{
    epsLabel->setText(QString("Eps = %1").arg(value));

    if (!freeSpace) return;

    // Update eps i ponovo racunaj reachability
    freeSpace->setEps(value);
    freeSpace->computeReachability();
    freeSpaceCanvas->update();
}
