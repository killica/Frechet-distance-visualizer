#include "FrechetDecision.h"
#include <cmath>
#include <algorithm>

FrechetDecision::FrechetDecision(const Polyline& p, const Polyline& q, double epsilon)
    : P(p), Q(q), eps(epsilon)
{
    int n = P.size() - 1;
    int m = Q.size() - 1;
    horizontal.resize(n, std::vector<CellInterval>(m));
    vertical.resize(n, std::vector<CellInterval>(m));

    // Precompute free-space intervals
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < m; ++j)
            computeCellIntervals(i, j);
}

// Solve quadratic equation a*x^2 + b*x + c <= 0, intersected with [0,1]
FrechetDecision::CellInterval FrechetDecision::solveQuadratic(double a, double b, double c) {
    // Ensure a != 0
    if(std::abs(a) < 1e-9) {
        if(std::abs(b) < 1e-9) {
            return (c <= 0) ? CellInterval(0,1) : CellInterval();
        }
        double t = -c/b;
        if(b > 0) return CellInterval(0, std::min(t,1.0));
        else       return CellInterval(std::max(t,0.0), 1.0);
    }

    double disc = b*b - 4*a*c;
    if(disc < 0) return CellInterval(); // empty
    double sqrtD = std::sqrt(disc);
    double t1 = (-b - sqrtD)/(2*a);
    double t2 = (-b + sqrtD)/(2*a);
    double mn = std::max(0.0, std::min(t1,t2));
    double mx = std::min(1.0, std::max(t1,t2));
    if(mn > mx) return CellInterval(); // no valid interval
    return CellInterval(mn,mx);
}

// Compute horizontal and vertical intervals for cell (i,j)
void FrechetDecision::computeCellIntervals(int i, int j) {
    const Point& p0 = P.vertices[i];
    const Point& p1 = P.vertices[i+1];
    const Point& q0 = Q.vertices[j];
    const Point& q1 = Q.vertices[j+1];

    // Compute coefficients for quadratic ||(1-s)p0+s*p1 - q0||^2 - eps^2 <= 0
    double dx = p1.x() - p0.x();
    double dy = p1.y() - p0.y();
    double cx = p0.x() - q0.x();
    double cy = p0.y() - q0.y();

    double a = dx*dx + dy*dy;
    double b = 2*(cx*dx + cy*dy);
    double c = cx*cx + cy*cy - eps*eps;

    horizontal[i][j] = solveQuadratic(a,b,c);

    // Similarly for vertical intervals along Q
    dx = q1.x() - q0.x();
    dy = q1.y() - q0.y();
    cx = q0.x() - p0.x();
    cy = q0.y() - p0.y();
    a = dx*dx + dy*dy;
    b = 2*(cx*dx + cy*dy);
    c = cx*cx + cy*cy - eps*eps;

    vertical[i][j] = solveQuadratic(a,b,c);
}

// Propagate reachable intervals through the grid
bool FrechetDecision::propagateFreeSpace() {
    int n = P.size() - 1;
    int m = Q.size() - 1;

    // DP table: reachable horizontal intervals
    std::vector<std::vector<CellInterval>> reachH(n, std::vector<CellInterval>(m));
    std::vector<std::vector<CellInterval>> reachV(n, std::vector<CellInterval>(m));

    // Start: donji levi ugao
    if(horizontal[0][0].minVal == 0)
        reachH[0][0] = horizontal[0][0];
    if(vertical[0][0].minVal == 0)
        reachV[0][0] = vertical[0][0];

    for(int i=0;i<n;++i){
        for(int j=0;j<m;++j){
            // Horizontal propagation (from left)
            if(j>0 && reachH[i][j-1].maxVal >= 0 && horizontal[i][j].minVal <= 1){
                double mn = std::max(horizontal[i][j].minVal, reachH[i][j-1].minVal);
                double mx = std::min(horizontal[i][j].maxVal, reachH[i][j-1].maxVal);
                if(mn <= mx) reachH[i][j] = CellInterval(mn,mx);
            }

            // Vertical propagation (from below)
            if(i>0 && reachV[i-1][j].maxVal >= 0 && vertical[i][j].minVal <= 1){
                double mn = std::max(vertical[i][j].minVal, reachV[i-1][j].minVal);
                double mx = std::min(vertical[i][j].maxVal, reachV[i-1][j].maxVal);
                if(mn <= mx) reachV[i][j] = CellInterval(mn,mx);
            }

            // Diagonal propagation (from bottom-left)
            if(i>0 && j>0
                && reachH[i-1][j-1].maxVal >= 0
                && reachV[i-1][j-1].maxVal >= 0){

                // Intersection of previous H and V intervals
                double mnH = std::max(horizontal[i][j].minVal, reachH[i-1][j-1].minVal);
                double mxH = std::min(horizontal[i][j].maxVal, reachH[i-1][j-1].maxVal);

                double mnV = std::max(vertical[i][j].minVal, reachV[i-1][j-1].minVal);
                double mxV = std::min(vertical[i][j].maxVal, reachV[i-1][j-1].maxVal);

                if(mnH <= mxH && mnV <= mxV){
                    reachH[i][j] = CellInterval(mnH, mxH);
                    reachV[i][j] = CellInterval(mnV, mxV);
                }
            }
        }
    }

    // Check if top-right corner reachable
    return reachH[n-1][m-1].maxVal >= 0 && reachV[n-1][m-1].maxVal >= 0;
}


// Main function
bool FrechetDecision::isReachable() {
    return propagateFreeSpace();
}
