#include "polylinecanvas.h"
#include <QPainter>

PolylineCanvas::PolylineCanvas(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(400, 300);
}

void PolylineCanvas::setPolylines(const Polyline& p, const Polyline& q) {
    P = p;
    Q = q;
    update();
}

void PolylineCanvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i + 1 < P.size(); ++i)
        painter.drawLine(P.vertices[i], P.vertices[i + 1]);

    painter.setPen(QPen(Qt::red, 2));
    for (int i = 0; i + 1 < Q.size(); ++i)
        painter.drawLine(Q.vertices[i], Q.vertices[i + 1]);
}
