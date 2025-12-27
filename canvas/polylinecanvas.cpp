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

    offsetX_ = margin + (w - (bb.maxX - bb.minX)*scale_) / 2.0 - bb.minX*scale_;
    offsetY_ = margin + (h - (bb.maxY - bb.minY)*scale_) / 2.0 + bb.minY*scale_;
}

QPointF PolylineCanvas::transformPoint(const Point& pt) const {
    double x = offsetX_ + pt.x() * scale_;
    double y = offsetY_ + (computeBoundingBox().maxY - pt.y()) * scale_;
    return QPointF(x, y);
}


void PolylineCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    if (P.vertices.empty() || Q.vertices.empty())
        return;

    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i + 1 < P.vertices.size(); ++i)
        painter.drawLine(transformPoint(P.vertices[i]),
                         transformPoint(P.vertices[i + 1]));

    painter.setPen(QPen(Qt::red, 2));
    for (int i = 0; i + 1 < Q.vertices.size(); ++i)
        painter.drawLine(transformPoint(Q.vertices[i]),
                         transformPoint(Q.vertices[i + 1]));

    // labeling points
    painter.setPen(QPen(Qt::blue, 2));
    QFont font = painter.font();
    font.setPointSizeF(12);
    painter.setFont(font);

    for (size_t i = 0; i < P.vertices.size(); ++i) {
        QPointF pt = transformPoint(P.vertices[i]);
        painter.drawText(pt + QPointF(5, -5), QString("P%1").arg(i));
    }

    painter.setPen(QPen(Qt::red, 2));
    for (size_t i = 0; i < Q.vertices.size(); ++i) {
        QPointF pt = transformPoint(Q.vertices[i]);
        painter.drawText(pt + QPointF(5, -5), QString("Q%1").arg(i));
    }
}


PolylineCanvas::BoundingBox PolylineCanvas::computeBoundingBox() const
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
