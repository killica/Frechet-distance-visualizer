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

    painter.save();
    int W = width();
    int H = height();

    double scaleX = 1.0;
    double scaleY = 1.0;

    double offsetX = 30; // left margin
    double offsetY = H - 50; // bottom margin

    auto transformPoint = [&](const QPointF& pt) -> QPointF {
        double x = pt.x() * scaleX + offsetX;
        double y = offsetY - pt.y() * scaleY; // invert y
        return QPointF(x, y);
    };

    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i + 1 < P.size(); ++i)
        painter.drawLine(transformPoint(P.vertices[i]),
                         transformPoint(P.vertices[i + 1]));

    painter.setPen(QPen(Qt::red, 2));
    for (int i = 0; i + 1 < Q.size(); ++i)
        painter.drawLine(transformPoint(Q.vertices[i]),
                         transformPoint(Q.vertices[i + 1]));

    painter.restore();
}
