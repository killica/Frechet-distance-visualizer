#include "polylinecanvas.h"
#include "freespacecanvas.h"
#include <QPainter>
#include <QTimer>
#include <QFileDialog>
#include <QRegularExpression>

PolylineCanvas::PolylineCanvas(QWidget *parent)
    : QWidget{parent}
{    
    setMinimumSize(400, 300);

    dogPixmap = QPixmap(":/images/doggo.png");
    humanPixmap = QPixmap(":/images/human.png");

    double dogPixScale = 0.12 * width() / dogPixmap.width();
    dogPixmapScaled = dogPixmap.scaled(dogPixmap.width()*dogPixScale,
                                       dogPixmap.height()*dogPixScale,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    double humanPixScale = 0.06 * width() / humanPixmap.width();
    humanPixmapScaled = humanPixmap.scaled(humanPixmap.width()*humanPixScale,
                                           humanPixmap.height()*humanPixScale,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);

    animTimer = new QTimer(this);
    animTimer->setInterval(50);
    connect(animTimer, &QTimer::timeout, this, &PolylineCanvas::updateAnimation);
}

Point PolylineCanvas::pointOnPolyline(const Polyline& poly, double param)
{
    int i = static_cast<int>(std::floor(param));
    double alpha = param - i;

    if (i < 0)
        return poly.vertices.front();

    if (i >= static_cast<int>(poly.vertices.size()) - 1)
        return poly.vertices.back();

    const Point& A = poly.vertices[i];
    const Point& B = poly.vertices[i + 1];

    return Point(
        (1.0 - alpha) * A.x() + alpha * B.x(),
        (1.0 - alpha) * A.y() + alpha * B.y()
    );
}


void PolylineCanvas::generateAnimationPositions(const std::vector<QPointF>& criticalPath)
{
    dogPositions.clear();
    humanPositions.clear();

    if (criticalPath.size() < 2)
        return;

    const int samplesPerSegment = 30;

    for (size_t k = 0; k + 1 < criticalPath.size(); ++k)
    {
        QPointF a = criticalPath[k];
        QPointF b = criticalPath[k + 1];

        for (int i = 0; i <= samplesPerSegment; ++i)
        {
            double t = static_cast<double>(i) / samplesPerSegment;

            double sParam = (1 - t) * a.x() + t * b.x();
            double qParam = (1 - t) * a.y() + t * b.y();

            Point dogPos = pointOnPolyline(P, sParam);
            Point humanPos = pointOnPolyline(Q, qParam);

            dogPositions.push_back(transformPoint(dogPos));
            humanPositions.push_back(transformPoint(humanPos));
        }
    }
}


void PolylineCanvas::startAnimation()
{
    if (dogPositions.empty() || humanPositions.empty()) return;

    animIndex = 0;
    currentDogPos = dogPositions[0];
    currentHumanPos = humanPositions[0];
    animTimer->start();
}

void PolylineCanvas::updateAnimation()
{
    if (animIndex < static_cast<int>(dogPositions.size())) {
        currentDogPos = dogPositions[animIndex];
        currentHumanPos = humanPositions[animIndex];
        ++animIndex;
        update();
    } else {
        animTimer->stop();
    }
}


void PolylineCanvas::setPolylines(const Polyline& p, const Polyline& q) {
    P = p;
    Q = q;
    updateTransform();

    if (!P.vertices.empty())
        currentDogPos = transformPoint(P.vertices[0]);
    if (!Q.vertices.empty())
        currentHumanPos = transformPoint(Q.vertices[0]);

    update();
}

void PolylineCanvas::updateTransform()
{
    if (P.vertices.empty() || Q.vertices.empty()) return;

    bbox_ = computeBoundingBox();

    int margin = 20;
    double w = width()  - 2 * margin;
    double h = height() - 2 * margin;

    double scaleX = w / (bbox_.maxX - bbox_.minX);
    double scaleY = h / (bbox_.maxY - bbox_.minY);
    scale_ = std::min(scaleX, scaleY);

    offsetX_ = margin - bbox_.minX * scale_;
    offsetY_ = margin + bbox_.maxY * scale_;
}


QPointF PolylineCanvas::transformPoint(const Point& pt) const
{
    double x = offsetX_ + pt.x() * scale_;
    double y = offsetY_ - pt.y() * scale_;
    return QPointF(x, y);
}

constexpr double MARKER_RADIUS = 3.0;

void PolylineCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    if (P.vertices.empty() || Q.vertices.empty())
        return;

    // --- crtanje linija ---
    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i + 1 < P.vertices.size(); ++i)
        painter.drawLine(transformPoint(P.vertices[i]),
                         transformPoint(P.vertices[i + 1]));

    painter.setPen(QPen(Qt::red, 2));
    for (int i = 0; i + 1 < Q.vertices.size(); ++i)
        painter.drawLine(transformPoint(Q.vertices[i]),
                         transformPoint(Q.vertices[i + 1]));

    QFont font = painter.font();
    font.setPointSizeF(12);
    painter.setFont(font);

    painter.setPen(Qt::blue);
    painter.setBrush(Qt::blue);
    for (size_t i = 0; i < P.vertices.size(); ++i) {
        QPointF pt = transformPoint(P.vertices[i]);
        painter.drawText(pt + QPointF(5, -5), QString("P%1").arg(FreeSpaceCanvas::subscriptNumber(i)));
        painter.drawEllipse(pt, MARKER_RADIUS, MARKER_RADIUS);
    }

    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    for (size_t i = 0; i < Q.vertices.size(); ++i) {
        QPointF pt = transformPoint(Q.vertices[i]);
        painter.drawText(pt + QPointF(5, -5), QString("Q%1").arg(FreeSpaceCanvas::subscriptNumber(i)));
        painter.drawEllipse(pt, MARKER_RADIUS, MARKER_RADIUS);
    }

    if (!dogPixmap.isNull() && dogPixmapScaled.isNull()) {
        double pixScale = 0.05 * width() / dogPixmap.width();
        dogPixmapScaled = dogPixmap.scaled(dogPixmap.width()*pixScale,
                                           dogPixmap.height()*pixScale,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
    }

    if (!humanPixmap.isNull() && humanPixmapScaled.isNull()) {
        double pixScale = 0.05 * width() / humanPixmap.width();
        humanPixmapScaled = humanPixmap.scaled(humanPixmap.width()*pixScale,
                                               humanPixmap.height()*pixScale,
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);
    }

    // leash drawing -> a bit translated so it fits the hand of a man and a collar of a dog
    if (currentDogPos != transformPoint(P.vertices[0]) || currentHumanPos != transformPoint(Q.vertices[0])) {
        painter.setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPointF(currentDogPos.x() + 2, currentDogPos.y()), QPointF(currentHumanPos.x() + 11, currentHumanPos.y()));
    }

    if (!dogPixmapScaled.isNull())
        painter.drawPixmap(currentDogPos.x() - dogPixmapScaled.width()/2,
                           currentDogPos.y() - dogPixmapScaled.height()/2,
                           dogPixmapScaled);

    if (!humanPixmapScaled.isNull())
        painter.drawPixmap(currentHumanPos.x() - humanPixmapScaled.width()/2,
                           currentHumanPos.y() - humanPixmapScaled.height()/2,
                           humanPixmapScaled);
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

void PolylineCanvas::restartAnimation()
{
    animIndex = 0;
    animTimer->start();
}

void PolylineCanvas::resetAnimation()
{
    if (animTimer->isActive()) {
        animTimer->stop();
    }

    dogPositions.clear();
    humanPositions.clear();
    animIndex = 0;

    if (!P.vertices.empty())
        currentDogPos = transformPoint(P.vertices[0]);
    if (!Q.vertices.empty())
        currentHumanPos = transformPoint(Q.vertices[0]);

    update();
}

void PolylineCanvas::loadPolylines()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open polyline file", "", "Text files (*.txt)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << fileName;
        return;
    }

    QTextStream in(&file);

    Polyline newP, newQ;
    Polyline* current = nullptr;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line == "P") {
            current = &newP;
            continue;
        }

        if (line == "Q") {
            current = &newQ;
            continue;
        }

        if (!current) {
            qWarning() << "Point without polyline header:" << line;
            continue;
        }

        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() != 2) {
            qWarning() << "Invalid line:" << line;
            continue;
        }

        bool okX, okY;
        double x = parts[0].toDouble(&okX);
        double y = parts[1].toDouble(&okY);

        if (!okX || !okY) {
            qWarning() << "Invalid coordinates:" << line;
            continue;
        }

        current->vertices.emplace_back(x, y);
    }

    if (newP.vertices.size() < 2 || newQ.vertices.size() < 2) {
        qWarning() << "Each polyline must have at least 2 points";
        return;
    }

    setPolylines(newP, newQ);

    file.close();

    emit polylinesLoaded(P, Q);
}

