#include "utils.h"
#include <algorithm>
#include "consts.h"

void remove_unordered(std::vector<Herring*>& vec, Herring* value) {
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        *it = std::move(vec.back());
        vec.pop_back();
    }
}

void avoidTank(const Vec3& s, const Vec3& v, Vec3& a) {
    for (int dimension = 0; dimension<dimensions; dimension ++) {
        Vec3 reflection_vector;
        for (int d = 0; d < dimensions; d++) {
            if (d == dimension) { reflection_vector.arr[d] = -v.arr[d]; }
            else { reflection_vector.arr[d] = v.arr[d]; }
        }
        Vec3 reflection_pos;
        if (v.arr[dimension] < 0) {
            reflection_pos.arr[dimension] = 0;
        } else {
            reflection_pos.arr[dimension] = tank_size;
        }
        for (int other_d = 0; other_d < dimensions; other_d ++) {
            if (other_d != dimension) {
                float divisor = (reflection_pos.arr[dimension] - v.arr[dimension]);
                if (abs(divisor) < epsilon) { divisor = epsilon; }
                reflection_pos.arr[other_d] = (s.arr[dimension] * v.arr[other_d])/divisor + s.arr[other_d];
            }
        }
        float abs_distance = (reflection_pos - s).abs();
        if (abs_distance < epsilon) {
            abs_distance = epsilon;
        }
        float scalar_collision = r_p/std::pow(abs_distance,p) + r_q/std::pow(abs_distance,q);
        if (std::isfinite(scalar_collision)) {
            a = a - (v - reflection_vector) * (gamma * scalar_collision);
        }
    }
}