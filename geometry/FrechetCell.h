#pragma once

#include <QString>

#pragma once
#include <QString>

struct Interval {
    double start;
    double end;
    bool empty;

    Interval() : start(0.0), end(0.0), empty(true) {}

    Interval(double s, double e) {
        if (s > e) {
            start = 0.0;
            end = 0.0;
            empty = true;
        } else {
            start = s;
            end = e;
            empty = false;
        }
    }

    Interval& operator=(const Interval& other) {
        if (this != &other) {
            start = other.start;
            end = other.end;
            empty = other.empty;
        }
        return *this;
    }

    bool isEmpty() const { return empty; }

    bool contains(double a) const {
        return start <= a && a <= end;
    }

    QString toString() const {
        return empty ? "∅" : QString("[%1,%2]").arg(start,0,'f',3).arg(end,0,'f',3);
    }
};


struct FrechetCell {
    Interval bottom, top, left, right;  // free-space
    Interval reachableBottom, reachableTop, reachableLeft, reachableRight;  // DP
    enum class PredOrigin {
        None,   // no predecessor
        Left,   // predecessor is the left edge
        Bottom  // predecessor is the right edge
    };

    PredOrigin predTop = PredOrigin::None;
    PredOrigin predRight = PredOrigin::None;


};
