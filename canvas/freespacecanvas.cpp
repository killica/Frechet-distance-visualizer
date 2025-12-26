#include "freespacecanvas.h"
#include <QPainter>

FreeSpaceCanvas::FreeSpaceCanvas(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 400);
}

void FreeSpaceCanvas::setFreeSpace(const FreeSpace* fs)
{
    fs_ = fs;
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

    int gridWidth  = n * s;
    int gridHeight = m * s;

    int offsetX = (width()  - gridWidth)  / 2;
    int offsetY = (height() - gridHeight) / 2;

    p.translate(offsetX, offsetY);

    // 1) mreža (crno)
    p.setPen(QPen(Qt::black, 1));

    for (int j = 0; j <= n; ++j)
        p.drawLine(j * s, 0, j * s, m * s);

    for (int i = 0; i <= m; ++i)
        p.drawLine(0, i * s, n * s, i * s);

    // 2) free space (plavo)
    p.setPen(QPen(Qt::blue, 3));

    const auto& cells = fs_->getCells();

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {

            const auto& c = cells[i][j];

            int x = i * s;
            int y = (m - 1 - j) * s;

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

    // 3) reachable intervali (zeleno)
    p.setPen(QPen(Qt::green, 4));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {

            const auto& c = cells[i][j];

            int x = i * s;
            int y = (m - 1 - j) * s;

            // bottom
            if (!c.reachableBottom.isEmpty()) {
                drawHorizontalInterval(p,
                                       c.reachableBottom.start,
                                       c.reachableBottom.end,
                                       x,
                                       y + s,
                                       s);
            }

            // top
            if (!c.reachableTop.isEmpty()) {
                drawHorizontalInterval(p,
                                       c.reachableTop.start,
                                       c.reachableTop.end,
                                       x,
                                       y,
                                       s);
            }

            // left
            if (!c.reachableLeft.isEmpty()) {
                drawVerticalInterval(p,
                                     c.reachableLeft.start,
                                     c.reachableLeft.end,
                                     x,
                                     y,
                                     s);
            }

            // right
            if (!c.reachableRight.isEmpty()) {
                drawVerticalInterval(p,
                                     c.reachableRight.start,
                                     c.reachableRight.end,
                                     x + s,
                                     y,
                                     s);
            }
        }
    }
}
