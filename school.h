#ifndef school_H
#define school_H

#include <vector>
#include <array>
#include "vec3.h"

class Herring;

class School {
    public:
    std::vector<std::array<int, 3>> cells;
    std::vector<Herring*> herring;
    School();
    Vec3 average_s();
    Vec3 average_v();
};

#endif