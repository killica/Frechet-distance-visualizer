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
    void updateTransform();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Polyline P;
    Polyline Q;

    double scale_ = 1.0;
    double offsetX_ = 0.0;
    double offsetY_ = 0.0;

    struct BoundingBox {
        double minX, maxX;
        double minY, maxY;
    };

    BoundingBox computeBoundingBox();

signals:
};

#endif // POLYLINECANVAS_H

