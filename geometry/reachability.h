#pragma once
#include "freespace.h"

namespace Frechet {

// Računa reachable intervale za sve ćelije
class Reachability {
public:
    // Konstruktor uz FreeSpace objekat
    Reachability(FreeSpace& fs);

    // Pokreće DP propagaciju
    void compute();

private:
    FreeSpace& fs_;

    // Pomoćne funkcije
    Interval computeRightReachable(const FrechetCell& cell);
    Interval computeTopReachable(const FrechetCell& cell);
};

} // namespace Frechet
