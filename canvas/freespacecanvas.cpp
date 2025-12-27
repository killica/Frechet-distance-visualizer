#include "freespacecanvas.h"
#include <QPainter>

FreeSpaceCanvas::FreeSpaceCanvas(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 400);
}

void FreeSpaceCanvas::updateCellSize()
{
    if (!fs_)
        return;

    int m = fs_->getM();
    int n = fs_->getN();
    int margin = 20;

    if (m > 0 && n > 0)
        cellSize_ = std::min(
            (width()  - 2 * margin) / m,
            (height() - 2 * margin) / n
            );
}


void FreeSpaceCanvas::setFreeSpace(FreeSpace* fs)
{
    fs_ = fs;
    updateCellSize();
    update();
}

static void drawHorizontalInterval(
    QPainter& p,
    double start,
    double end,
    int x,
    int y,
    int s)
{
    int x1 = x + start * s;
    int x2 = x + end   * s;
    p.drawLine(x1, y, x2, y);
}

static void drawVerticalInterval(
    QPainter& p,
    double start,
    double end,
    int x,
    int y,
    int s)
{
    int y1 = y + s - end   * s;
    int y2 = y + s - start * s;
    p.drawLine(x, y1, x, y2);
}

void FreeSpaceCanvas::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (!fs_)
        return;

    int m = fs_->getM();
    int n = fs_->getN();
    int s = cellSize_;

    int gridWidth  = m * s;
    int gridHeight = n * s;

    // center inside widget
    int offsetX = (width()  - gridWidth)  / 2;
    int offsetY = (height() - gridHeight) / 2;

    p.save();
    p.translate(offsetX, offsetY);

    p.setPen(QPen(Qt::black, 1));
    for (int i = 0; i <= m; ++i)
        p.drawLine(i * s, 0, i * s, gridHeight);

    for (int j = 0; j <= n; ++j)
        p.drawLine(0, gridHeight - j * s, gridWidth, gridHeight - j * s);

    p.setPen(QPen(Qt::blue, 3));
    const auto& cells = fs_->getCells();

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            const auto& c = cells[i][j];
            int x = i * s;
            int y = gridHeight - (j + 1) * s; // bottom-left corner of cell

            if (!c.bottom.isEmpty())
                drawHorizontalInterval(p, c.bottom.start, c.bottom.end, x, y + s, s);
            if (!c.top.isEmpty())
                drawHorizontalInterval(p, c.top.start, c.top.end, x, y, s);
            if (!c.left.isEmpty())
                drawVerticalInterval(p, c.left.start, c.left.end, x, y, s);
            if (!c.right.isEmpty())
                drawVerticalInterval(p, c.right.start, c.right.end, x + s, y, s);
        }
    }

    p.setPen(QPen(Qt::green, 4));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            const auto& c = cells[i][j];
            int x = i * s;
            int y = gridHeight - (j + 1) * s;

            if (!c.reachableBottom.isEmpty())
                drawHorizontalInterval(p, c.reachableBottom.start, c.reachableBottom.end, x, y + s, s);
            if (!c.reachableTop.isEmpty())
                drawHorizontalInterval(p, c.reachableTop.start, c.reachableTop.end, x, y, s);
            if (!c.reachableLeft.isEmpty())
                drawVerticalInterval(p, c.reachableLeft.start, c.reachableLeft.end, x, y, s);
            if (!c.reachableRight.isEmpty())
                drawVerticalInterval(p, c.reachableRight.start, c.reachableRight.end, x + s, y, s);
        }
    }

    if (fs_->pathComputed && fs_->getEps() >= fs_->criticalEps) {
        p.setPen(QPen(Qt::yellow, 2));

        for (size_t k = 0; k + 1 < fs_->criticalPath.size(); ++k) {
            const QPointF& p1 = fs_->criticalPath[k];
            const QPointF& p2 = fs_->criticalPath[k+1];

            int x1 = static_cast<int>(p1.x() * s);
            int y1 = static_cast<int>(gridHeight - p1.y() * s);
            int x2 = static_cast<int>(p2.x() * s);
            int y2 = static_cast<int>(gridHeight - p2.y() * s);

            p.drawLine(x1, y1, x2, y2);
        }
    }


    p.restore();
}
