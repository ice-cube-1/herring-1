#ifndef UTILS_H
#define UTILS_H
#pragma once
#include <vector>
#include "school.h"
#include "consts.h"

void remove_unordered(std::vector<Herring*>& vec, Herring* value);
void avoidTank(const Vec3& s, const Vec3& v, Vec3& a);
extern float heights[cell_count][cell_count];
void init_planes();
void avoid_floor_hard(Vec3& s, Vec3& v);
std::array<int, 3> get_cell(Vec3 s);
class Plane {
    public:
    int dimension;
    Plane();
    Plane(Vec3 min, Vec3 max, int d);
    float distance_to_intersection(Vec3 s, Vec3 d);
    private:
    Vec3 min_values;
    Vec3 max_values;
};
#endif