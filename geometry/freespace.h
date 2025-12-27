#pragma once
#include "FrechetCell.h"
#include "Polyline.h"
#include <vector>
#include <QLineF>

class FreeSpace {
public:
    FreeSpace(const Polyline& P, const Polyline& Q, double epsilon);
    inline int getM() const {return cells.size();}
    inline int getN() const {return cells[0].size();}
    inline double getEps() const {return eps;}

    const std::vector<std::vector<FrechetCell>>& getCells() const {
        return cells;
    }

    std::vector<std::vector<FrechetCell>>& getCells() {
        return cells;
    }

    void computeCells();

    void setEps(double e);
    void computeReachability();

    std::vector<QPointF> criticalPath;
    bool pathComputed = false;
    int criticalEps = -1;


    bool isTopRightReachable() const;
    std::vector<QPointF> computeCriticalPath();
private:
    Polyline P;
    Polyline Q;
    double eps;

    std::vector<std::vector<FrechetCell>> cells;

    Interval computeBottomInterval(const Point& p0, const Point& p1, const Point& q0);
    Interval computeTopInterval(const Point& p0, const Point& p1, const Point& q1);
    Interval computeLeftInterval(const Point& q0, const Point& q1, const Point& p0);
    Interval computeRightInterval(const Point& q0, const Point& q1, const Point& p1);
};
