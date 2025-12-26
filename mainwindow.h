#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "canvas/polylinecanvas.h"
#include "canvas/freespacecanvas.h"
#include <QMainWindow>
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

};
#endif // MAINWINDOW_H
