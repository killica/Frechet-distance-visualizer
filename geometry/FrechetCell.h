#pragma once

#include <QString>

#pragma once
#include <QString>

struct Interval {
    double start;
    double end;
    bool empty;

    // Konstruktor za prazan interval
    Interval() : start(0.0), end(0.0), empty(true) {}

    // Konstruktor sa start i end
    Interval(double s, double e) {
        if (s >= e) {
            // Ako levi >= desnog, interval je prazan
            start = 0.0;
            end = 0.0;
            empty = true;
        } else {
            start = s;
            end = e;
            empty = false;
        }
    }

    // Provera da li je interval prazan
    bool isEmpty() const { return empty; }

    // Ispis intervala za debug
    QString toString() const {
        return empty ? "∅" : QString("[%1,%2]").arg(start,0,'f',3).arg(end,0,'f',3);
    }
};


struct FrechetCell {
    Interval bottom, top, left, right;  // free-space
    Interval reachableBottom, reachableTop, reachableLeft, reachableRight;  // DP
};
