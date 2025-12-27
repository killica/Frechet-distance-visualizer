#pragma once
#include "freespace.h"

namespace Frechet {

class Reachability {
public:
    Reachability(FreeSpace& fs);

    // Initiate DP propagation
    void compute();

private:
    FreeSpace& fs_;

    Interval computeRightReachable(const FrechetCell& cell);
    Interval computeTopReachable(const FrechetCell& cell);
};

} // namespace Frechet
