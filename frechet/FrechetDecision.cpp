#include "FrechetDecision.h"
#include <cmath>

FrechetDecision::FrechetDecision(const Polyline& p,
                                 const Polyline& q,
                                 double epsilon)
    : P(p), Q(q), eps(epsilon) {}

bool FrechetDecision::cellFree(int i, int j) {
    // TODO: logic for checking if segments P[i]-P[i+1] and Q[j]-Q[j+1] have free-space
    return true; // dummy for now
}

bool FrechetDecision::isReachable() {
    int n = P.size();
    int m = Q.size();

    // TODO: free-space propagation
    return true; // dummy for now
}
