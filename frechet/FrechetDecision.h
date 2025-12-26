#pragma once
#include "../geometry/Polyline.h"
#include <vector>
#include <utility>

class FrechetDecision {
public:
    // Constructor: takes two polylines and epsilon
    FrechetDecision(const Polyline& p, const Polyline& q, double epsilon);

    // Returns true if Fréchet distance between P and Q <= epsilon
    bool isReachable();

private:
    const Polyline& P;
    const Polyline& Q;
    double eps;

    struct CellInterval {
        // interval in s or t direction where distance <= epsilon
        double minVal;
        double maxVal;
        bool empty;
        CellInterval() : minVal(0), maxVal(0), empty(true) {}
        CellInterval(double mn, double mx) : minVal(mn), maxVal(mx), empty(false) {}
    };

    std::vector<std::vector<CellInterval>> horizontal; // intervals along P
    std::vector<std::vector<CellInterval>> vertical;   // intervals along Q

    // Compute free-space intervals for one cell
    void computeCellIntervals(int i, int j);

    // Solve quadratic for distance <= eps, return interval in [0,1]
    CellInterval solveQuadratic(double a, double b, double c);

    // Propagate reachable intervals through the grid
    bool propagateFreeSpace();
};
