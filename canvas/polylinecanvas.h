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

    QPointF transformPoint(const Point &pt) const;
    void startAnimation();
    void updateAnimation();
    void generateAnimationPositions(const std::vector<QPointF>& criticalPath);

    Point pointOnPolyline(const Polyline &poly, double param);
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

    BoundingBox bbox_;
    BoundingBox computeBoundingBox() const;

    // Animation info
    QPixmap dogPixmap, dogPixmapScaled;
    QPixmap humanPixmap, humanPixmapScaled;
    QPointF currentDogPos;
    QPointF currentHumanPos;

    QVector<QPointF> dogPositions;
    QVector<QPointF> humanPositions;
    int animIndex = 0;
    bool animationRunning = false;
    QTimer* animTimer = nullptr;

public slots:
    void restartAnimation();
    void loadPolylines();

signals:
    void polylinesLoaded(const Polyline& P, const Polyline& Q);

};

#endif // POLYLINECANVAS_H

