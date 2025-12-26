#include "reachability.h"
#include <algorithm>

namespace Frechet {

Reachability::Reachability(FreeSpace& fs) : fs_(fs) {}

void Reachability::compute() {
    int m = fs_.getM(); // broj segmenta P
    int n = fs_.getN(); // broj segmenta Q

    auto& cells = fs_.getCells();

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            FrechetCell& c = cells[i][j];

            // reachableBottom
            if (j == 0) {
                c.reachableBottom = c.bottom;
            } else {
                c.reachableBottom = cells[i][j-1].reachableTop;
            }

            // reachableLeft
            if (i == 0) {
                c.reachableLeft = c.left;
            } else {
                c.reachableLeft = cells[i-1][j].reachableRight;
            }

            // reachableTop
            c.reachableTop = computeTopReachable(c);

            // reachableRight
            c.reachableRight = computeRightReachable(c);
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
