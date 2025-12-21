#pragma once
#include "../geometry/Polyline.h"

class FrechetDecision {
public:
    FrechetDecision(const Polyline& p,
                    const Polyline& q,
                    double epsilon);

    bool isReachable(); // true if Fréchet(P,Q) ≤ epsilon

private:
    const Polyline& P;
    const Polyline& Q;
    double eps;

    bool cellFree(int i, int j);
};
