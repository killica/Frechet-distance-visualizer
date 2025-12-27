#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QPushButton>

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

    restartAnimButton = new QPushButton("Restart animation");
    restartAnimButton->setEnabled(false);
    connect(restartAnimButton, &QPushButton::clicked,
            polylineCanvas, &PolylineCanvas::restartAnimation);

    loadPolylinesButton = new QPushButton("Load polylines");
    connect(loadPolylinesButton, &QPushButton::clicked,
            polylineCanvas, &PolylineCanvas::loadPolylines);

    auto* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(loadPolylinesButton);
    buttonsLayout->addWidget(restartAnimButton);

    leftLayout->addWidget(polylineTitle);
    leftLayout->addWidget(polylineCanvas, 1);
    leftLayout->addLayout(buttonsLayout, 0);

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
    epsSlider->setMinimum(0);
    epsSlider->setMaximum(100);
    epsSlider->setValue(0);

    epsLabel = new QLabel(QString("ε = %1").arg(epsSlider->value()));
    epsLabel->setAlignment(Qt::AlignLeft);

    criticalEpsLabel = new QLabel(QString("Critical ε = ?"));
    criticalEpsLabel->setAlignment(Qt::AlignRight);

    auto* epsLayout = new QHBoxLayout();
    epsLayout->addWidget(epsLabel);
    epsLayout->addWidget(criticalEpsLabel);

    rightLayout->addLayout(epsLayout);
    rightLayout->addWidget(epsSlider);

    mainLayout->addLayout(rightLayout, 1);

    // --- Polylines start empty ---
    Polyline P, Q; // prazni
    polylineCanvas->setPolylines(P, Q);

    // freeSpace se ne kreira dok korisnik ne učita fajl
    freeSpace.reset();

    freeSpaceCanvas->setFreeSpace(nullptr);

    // --- Signals & slots ---
    connect(epsSlider, &QSlider::valueChanged,
            this, &MainWindow::onEpsChanged);

    connect(polylineCanvas, &PolylineCanvas::polylinesLoaded,
            this, &MainWindow::onPolylinesLoaded);

    setWindowTitle("Frechet Distance Visualizer");
    resize(1100, 650);
}


MainWindow::~MainWindow()
{
    delete ui;
}

// Slot
void MainWindow::onEpsChanged(int value)
{
    epsLabel->setText(QString("ε = %1").arg(value));

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

        polylineCanvas->generateAnimationPositions(freeSpace->criticalPath);
        polylineCanvas->startAnimation();
        restartAnimButton->setEnabled(true);
        criticalEpsLabel->setText(QString("Critical ε ≈ %1").arg(value));
        criticalEpsLabel->setStyleSheet(
            "color: darkgreen;"
            "font-size: 17px;"
            "font-weight: bold;"
            );
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

void MainWindow::onPolylinesLoaded(const Polyline& P, const Polyline& Q)
{
    polylineCanvas->setPolylines(P, Q);

    freeSpace.reset();

    if (!P.vertices.empty() && !Q.vertices.empty()) {
        freeSpace = std::make_unique<FreeSpace>(P, Q, epsSlider->value());
        freeSpaceCanvas->setFreeSpace(freeSpace.get());
    }

    criticalEpsLabel->setText("Critical ε = ?");
    criticalEpsLabel->setStyleSheet("");
    epsSlider->setValue(0);
    restartAnimButton->setEnabled(false);
}

