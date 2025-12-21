#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "geometry/Polyline.h"

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    void setPolylines(const Polyline& p, const Polyline& q);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Polyline P;
    Polyline Q;

signals:
};

#endif // CANVAS_H
