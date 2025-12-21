#pragma once
#include <vector>
#include "Point.h"

class Polyline {
public:
    std::vector<Point> vertices;

    Polyline() = default;
    explicit Polyline(const std::vector<Point>& v) : vertices(v) {}

    int size() const { return static_cast<int>(vertices.size()); }
};
