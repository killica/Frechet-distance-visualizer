#include "freespace.h"
#include "reachability.h"
#include "reachability.h"
#include <cmath>
#include <algorithm>

using std::max;
using std::min;

static double dot(const Point& a, const Point& b)
{
    return a.x() * b.x() + a.y() * b.y();
}

/*
   Solve inequality:
   || (1-s)A + sB - C ||^2 <= eps^2
   for s in [0,1]
*/
static Interval segmentPointInterval(
    const Point& A,
    const Point& B,
    const Point& C,
    double eps)
{
    Point d = B - A;
    Point f = A - C;

    double a = dot(d, d);
    double b = 2.0 * dot(d, f);
    double c = dot(f, f) - eps * eps;

    // Segment is a point
    if (std::abs(a) < 1e-12)
    {
        if (c <= 0.0)
            return Interval(0.0, 1.0);
        else
            return Interval(1.0, 0.0);
    }

    double disc = b * b - 4 * a * c;
    if (disc < 0.0)
        return Interval(1.0, 0.0);

    double sqrtDisc = std::sqrt(disc);
    double s1 = (-b - sqrtDisc) / (2 * a);
    double s2 = (-b + sqrtDisc) / (2 * a);

    double start = max(0.0, min(s1, s2));
    double end   = min(1.0, max(s1, s2));

    if (start > end)
        return Interval(1.0, 0.0);

    return Interval(start, end);
}

FreeSpace::FreeSpace(const Polyline& P_,
                     const Polyline& Q_,
                     double epsilon)
    : P(P_), Q(Q_), eps(epsilon)
{
    int n = P.vertices.size() - 1;
    int m = Q.vertices.size() - 1;

    cells.resize(n, std::vector<FrechetCell>(m));
    computeCells();
}

void FreeSpace::computeCells()
{
    int n = P.vertices.size() - 1;
    int m = Q.vertices.size() - 1;

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            const Point& p0 = P.vertices[i];
            const Point& p1 = P.vertices[i + 1];
            const Point& q0 = Q.vertices[j];
            const Point& q1 = Q.vertices[j + 1];

            FrechetCell& cell = cells[i][j];

            // bottom: P(s) vs Q[j]
            cell.bottom = segmentPointInterval(p0, p1, q0, eps);

            // top: P(s) vs Q[j+1]
            cell.top = segmentPointInterval(p0, p1, q1, eps);

            // left: Q(t) vs P[i]
            cell.left = segmentPointInterval(q0, q1, p0, eps);

            // right: Q(t) vs P[i+1]
            cell.right = segmentPointInterval(q0, q1, p1, eps);
        }
    }
}

void FreeSpace::setEps(double e) {
    eps = e;
    computeCells();
}

void FreeSpace::computeReachability() {
    Frechet::Reachability reach(*this);
    reach.compute();
}


bool FreeSpace::isTopRightReachable() const {
    int m = getM();
    int n = getN();

    if (m == 0 || n == 0) return false;

    const auto& topRightCell = cells[m-1][n-1];

    bool topOk = topRightCell.reachableTop.start <= 1.0 && 1.0 <= topRightCell.reachableTop.end;

    bool rightOk = topRightCell.reachableRight.start <= 1.0 && 1.0 <= topRightCell.reachableRight.end;

    return topOk || rightOk;
}

std::vector<QPointF> FreeSpace::computeCriticalPath() {
    std::vector<QPointF> path;

    int m = getM();
    int n = getN();
    auto& cells = getCells();

    if (m == 0 || n == 0) return path;

    // --- start in upper right corner ---
    int i = m - 1;
    int j = n - 1;
    FrechetCell* cell = &cells[i][j];

    bool onTop = !cell->reachableTop.isEmpty();
    bool onRight = !cell->reachableRight.isEmpty();
    if (!onTop && !onRight) return path;

    while (i >= 0 && j >= 0) {
        if (onTop) {
            double mid = 0.5 * (cell->reachableTop.start + cell->reachableTop.end);
            double x = i + mid;
            double y = j + 1; // top edge
            path.push_back(QPointF(x, y));

            if (cell->predTop == FrechetCell::PredOrigin::Left) {
                --i;
                if (i < 0) break;
                cell = &cells[i][j];
                onTop = false;
                onRight = true;
            } else if (cell->predTop == FrechetCell::PredOrigin::Bottom) {
                --j;
                if (j < 0) break;
                cell = &cells[i][j];
                onTop = true;
                onRight = false;
            } else {
                break; // None
            }
        } else if (onRight) {
            double mid = 0.5 * (cell->reachableRight.start + cell->reachableRight.end);
            double x = i + 1; // right edge
            double y = j + mid;
            path.push_back(QPointF(x, y));

            if (cell->predRight == FrechetCell::PredOrigin::Bottom) {
                --j;
                if (j < 0) break;
                cell = &cells[i][j];
                onTop = true;
                onRight = false;
            } else if (cell->predRight == FrechetCell::PredOrigin::Left) {
                --i;
                if (i < 0) break;
                cell = &cells[i][j];
                onTop = false;
                onRight = true;
            } else {
                break; // None
            }
        } else {
            break;
        }
    }

    if (path.empty() || path.front() != QPointF(0.0, 0.0))
        path.push_back(QPointF(0.0, 0.0));

    std::reverse(path.begin(), path.end());
    path.pop_back();
    path.push_back(QPointF(m, n));
    return path;
}
