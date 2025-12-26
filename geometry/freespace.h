#pragma once
#include "FrechetCell.h"
#include "Polyline.h" // pretpostavljamo da Polyline sadrži vector<Point>
#include <vector>

class FreeSpace {
public:
    FreeSpace(const Polyline& P, const Polyline& Q, double epsilon);
    inline int getM() {return cells.size();}
    inline int getN() {return cells[0].size();}

    std::vector<std::vector<FrechetCell>>& getCells() { return cells; }

private:
    const Polyline& P;
    const Polyline& Q;
    double eps;

    std::vector<std::vector<FrechetCell>> cells;

    void computeCells();
    Interval computeBottomInterval(const Point& p0, const Point& p1, const Point& q0);
    Interval computeTopInterval(const Point& p0, const Point& p1, const Point& q1);
    Interval computeLeftInterval(const Point& q0, const Point& q1, const Point& p0);
    Interval computeRightInterval(const Point& q0, const Point& q1, const Point& p1);
};
