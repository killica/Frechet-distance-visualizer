#ifndef POLYLINECANVAS_H
#define POLYLINECANVAS_H

#include <QWidget>
#include "../geometry/Polyline.h"

class PolylineCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit PolylineCanvas(QWidget *parent = nullptr);
    void setPolylines(const Polyline& p, const Polyline& q);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Polyline P;
    Polyline Q;

signals:
};

#endif // POLYLINECANVAS_H

