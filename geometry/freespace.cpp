#include "freespace.h"
#include <cmath>
#include <algorithm>

using std::max;
using std::min;

/* =========================
   Pomoćne funkcije
   ========================= */

static double dot(const Point& a, const Point& b)
{
    return a.x() * b.x() + a.y() * b.y();
}

/*
   Rešava nejednakost:
   || (1-s)A + sB - C ||^2 <= eps^2
   za s ∈ [0,1]
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

    // Degenerisan slučaj: segment je tačka
    if (std::abs(a) < 1e-12)
    {
        if (c <= 0.0)
            return Interval(0.0, 1.0);
        else
            return Interval(1.0, 0.0); // prazan
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

/* =========================
   FreeSpace implementacija
   ========================= */

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
