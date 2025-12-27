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
    updateTransform();
    update();
}

void PolylineCanvas::updateTransform()
{
    if (P.vertices.empty() || Q.vertices.empty()) return;

    BoundingBox bb = computeBoundingBox();
    int margin = 20;

    double w = width() - 2*margin;
    double h = height() - 2*margin;

    double scaleX = w / (bb.maxX - bb.minX);
    double scaleY = h / (bb.maxY - bb.minY);
    scale_ = std::min(scaleX, scaleY);

    // Centriranje: pomeramo tako da bbox bude u sredini widgeta
    offsetX_ = margin + (w - (bb.maxX - bb.minX)*scale_) / 2.0 - bb.minX*scale_;
    offsetY_ = margin + (h - (bb.maxY - bb.minY)*scale_) / 2.0 + bb.minY*scale_;
}


void PolylineCanvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    painter.save();

    painter.translate(offsetX_, height() - offsetY_);
    painter.scale(scale_, -scale_);

    // crtanje P i Q
    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i + 1 < P.size(); ++i)
        painter.drawLine(P.vertices[i], P.vertices[i + 1]);

    painter.setPen(QPen(Qt::red, 2));
    for (int i = 0; i + 1 < Q.size(); ++i)
        painter.drawLine(Q.vertices[i], Q.vertices[i + 1]);

    painter.restore();

}

PolylineCanvas::BoundingBox PolylineCanvas::computeBoundingBox()
{
    BoundingBox bb;
    bb.minX = bb.minY = 1e20;
    bb.maxX = bb.maxY = -1e20;

    auto update = [&](const Point& pt) {
        if (pt.x() < bb.minX) bb.minX = pt.x();
        if (pt.x() > bb.maxX) bb.maxX = pt.x();
        if (pt.y() < bb.minY) bb.minY = pt.y();
        if (pt.y() > bb.maxY) bb.maxY = pt.y();
    };

    for (const auto& p : P.vertices) update(p);
    for (const auto& q : Q.vertices) update(q);

    return bb;
}
