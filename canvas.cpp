#include "canvas.h"
#include <QPainter>


Canvas::Canvas(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(400, 300);
}

void Canvas::setPolylines(const Polyline& p, const Polyline& q) {
    P = p;
    Q = q;
    update();  // repaint
}

void Canvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    // Linija P plava
    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i + 1 < P.size(); ++i)
        painter.drawLine(P.vertices[i], P.vertices[i + 1]);

    // Linija Q crvena
    painter.setPen(QPen(Qt::red, 2));
    for (int i = 0; i + 1 < Q.size(); ++i)
        painter.drawLine(Q.vertices[i], Q.vertices[i + 1]);
}
