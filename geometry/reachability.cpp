#include "reachability.h"
#include <algorithm>

namespace Frechet {

Reachability::Reachability(FreeSpace& fs) : fs_(fs) {}

void Reachability::compute() {
    int m = fs_.getM();
    int n = fs_.getN();

    auto& cells = fs_.getCells();

    // calculate for bottom left cell
    if (cells[0][0].bottom.contains(0.0))
        cells[0][0].reachableBottom = cells[0][0].bottom;
    else
        cells[0][0].reachableBottom = Interval();

    if (cells[0][0].left.contains(0.0))
        cells[0][0].reachableLeft = cells[0][0].left;
    else
        cells[0][0].reachableLeft = Interval();

    // calculate for the rest of the bottom row
    for (int i = 1; i < m; ++i) {
        if (cells[i-1][0].reachableBottom.contains(1.0) && cells[i][0].bottom.contains(0.0))
            cells[i][0].reachableBottom = cells[i][0].bottom;
        else
            cells[i][0].reachableBottom = Interval();
    }

    // calculate for the rest of the left column
    for (int j = 1; j < n; ++j) {
        if (cells[0][j-1].reachableLeft.contains(1.0) && cells[0][j].left.contains(0.0))
            cells[0][j].reachableLeft = cells[0][j].left;
        else
            cells[0][j].reachableLeft = Interval();
    }

    // calculate for the rest of the cells
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            FrechetCell& c = cells[i][j];

            // reachableTop
            c.reachableTop = computeTopReachable(c);
            if (j < n - 1)
                cells[i][j+1].reachableBottom = c.reachableTop;
            if (!c.reachableLeft.isEmpty()) {
                c.predTop = FrechetCell::PredOrigin::Left;
            } else if (!c.reachableBottom.isEmpty()) {
                c.predTop = FrechetCell::PredOrigin::Bottom;
            } else {
                c.predTop = FrechetCell::PredOrigin::None;
            }

            // reachableRight
            c.reachableRight = computeRightReachable(c);
            if (i < m - 1)
                cells[i+1][j].reachableLeft = c.reachableRight;
            if (!c.reachableLeft.isEmpty())
                c.predRight = FrechetCell::PredOrigin::Left;
            else if (!c.reachableBottom.isEmpty())
                c.predRight = FrechetCell::PredOrigin::Bottom;
            else
                c.predRight = FrechetCell::PredOrigin::None;
        }
    }
}

Interval Reachability::computeTopReachable(const FrechetCell& cell) {
    if (!cell.reachableLeft.isEmpty()) {
        return cell.top;
    } else if (!cell.reachableBottom.isEmpty()) {
        double s = std::max(cell.reachableBottom.start, cell.top.start);
        double e = cell.top.end;
        return Interval(s,e);
    } else {
        return Interval();
    }
}

Interval Reachability::computeRightReachable(const FrechetCell& cell) {
    if (!cell.reachableBottom.isEmpty()) {
        return cell.right;
    } else if (!cell.reachableLeft.isEmpty()) {
        double s = std::max(cell.reachableLeft.start, cell.right.start);
        double e = cell.right.end;
        return Interval(s,e);
    } else {
        return Interval();
    }
}

} // namespace Frechet
