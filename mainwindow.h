#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "canvas/polylinecanvas.h"
#include "canvas/freespacecanvas.h"
#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Canvas;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PolylineCanvas* polylineCanvas;
    FreeSpaceCanvas* freeSpaceCanvas;
    std::unique_ptr<FreeSpace> freeSpace;
    QSlider* epsSlider;
    QLabel* epsLabel;
    QLabel* criticalEpsLabel;
    QPushButton* restartAnimButton;
    QPushButton* loadPolylinesButton;

private slots:
    void onEpsChanged(int value);
    void onPolylinesLoaded(const Polyline& P, const Polyline& Q);

};
#endif // MAINWINDOW_H
