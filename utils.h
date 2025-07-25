#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "school.h"

void remove_unordered(std::vector<Herring*>& vec, Herring* value);
void avoidTank(const Vec3& s, const Vec3& v, Vec3& a);
#endif