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

class Plane {
    public:
    int dimension;
    Plane() {}
    Plane(Vec3 min, Vec3 max, int d) { min_values = min; max_values = max; dimension = d; }

    float distance_to_intersection(Vec3 s, Vec3 v) {
        if ((v.arr[dimension] > 0 && s.arr[dimension] > min_values.arr[dimension]) ||
            (v.arr[dimension] < 0 && s.arr[dimension] < min_values.arr[dimension]))
            return -1;

        Vec3 meeting_point;
        meeting_point.arr[dimension] = min_values.arr[dimension];
        for (int i = 0; i<3; i++) {
            if (i!=dimension) {
                meeting_point.arr[i] = ((meeting_point.arr[dimension]-s.arr[dimension])*v.arr[i]/v.arr[dimension])+s.arr[i];
                if (meeting_point.arr[i] > max_values.arr[i] || meeting_point.arr[i] < min_values.arr[i]) {
                    return -1;
                }
            }
        }
        return (s-meeting_point).abs();
    }
    private:
    Vec3 min_values;
    Vec3 max_values;
};

void avoidTank(const Vec3& s, const Vec3& v, Vec3& a) {
    Plane planes[6] = {
        Plane(Vec3{0,0,0}, Vec3{0,tank_size,tank_size}, 0),
        Plane(Vec3{tank_size,0,0}, Vec3{tank_size,tank_size,tank_size}, 0),
        Plane(Vec3{0,0,0}, Vec3{tank_size,0,tank_size}, 1),
        Plane(Vec3{0,tank_size,0}, Vec3{tank_size,tank_size,tank_size}, 1),
        Plane(Vec3{0,0,0}, Vec3{tank_size,tank_size,0}, 2),
        Plane(Vec3{0,0,tank_size}, Vec3{tank_size,tank_size,tank_size}, 2)
    };
    Plane closest_plane;
    float closest_plane_dist = std::numeric_limits<float>::max();
    for (Plane plane: planes) {
        float dist_intersect = plane.distance_to_intersection(s,v);
        if (dist_intersect != -1 && dist_intersect < closest_plane_dist) {
            closest_plane = plane;
            closest_plane_dist = dist_intersect;
        }
    }

    if (closest_plane_dist > epsilon) { 
        Vec3 reflection_vector;
        for (int d = 0; d < dimensions; d++) {
            reflection_vector.arr[d] = (d == closest_plane.dimension) ? -v.arr[d] : v.arr[d];
        }

        float scalar_collision = r_p/std::pow(closest_plane_dist,p) + r_q/std::pow(closest_plane_dist,q);
        if (std::isfinite(scalar_collision)) {
            a = a - (v - reflection_vector) * (gamma * scalar_collision);
        }
    }
}