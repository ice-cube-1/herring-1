#include "utils.h"
#include <algorithm>
#include "consts.h"
#include <iostream>

void remove_unordered(std::vector<Herring*>& vec, Herring* value) {
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        *it = std::move(vec.back());
        vec.pop_back();
    }
}

Plane::Plane() {}
Plane::Plane(Vec3 min, Vec3 max, int d) { min_values = min; max_values = max; dimension = d; }

float Plane::distance_to_intersection(Vec3 s, Vec3 v) {
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


std::vector<Plane> planes =  {
        Plane(Vec3{0,0,0}, Vec3{0,tank_size,tank_size}, 0),
        Plane(Vec3{tank_size,0,0}, Vec3{tank_size,tank_size,tank_size}, 0),
        Plane(Vec3{0,0,0}, Vec3{tank_size,0,tank_size}, 1),
        Plane(Vec3{0,tank_size,0}, Vec3{tank_size,tank_size,tank_size}, 1),
        Plane(Vec3{0,0,0}, Vec3{tank_size,tank_size,0}, 2),
        Plane(Vec3{0,0,tank_size}, Vec3{tank_size,tank_size,tank_size}, 2)
    };

float heights[cell_count][cell_count];

void init_planes() {
    for (int i = 0; i<cell_count; i++) {
        for (int j = 0; j<cell_count; j++) {
            heights[i][j] = std::abs(distribution(generator))*3.0f;
            std::cout<<heights[i][j];
        }
    }
}

void avoid_floor_hard(Vec3& s, Vec3& v) {
    std::array<int, 3> initial_cell = get_cell(s);
    std::array<int, 3> current_cell = initial_cell;
    Vec3 current_position = s;
    int count = 0;
    while (heights[current_cell[0]][current_cell[1]] > current_position.z() && count < 15) {
        count ++;
        bool out=false;
        for (float val : current_cell) {
            if (val < 0 || val >= cell_count) {
                out=true;
            }
        }
        if (out) break;
        current_position = current_position - (v*0.1f*d_t);
        current_cell = get_cell(current_position);
        count ++;
    }
    if (current_position != s) {
        s = current_position;
        if (initial_cell[0] != current_cell[0]) {
            v.arr[0] = -v.arr[0];
        } else if (initial_cell[1] != current_cell[1]) {
            v.arr[1] = -v.arr[1];
        } else {
            v.arr[2] = -v.arr[2];
        }
        s = current_position;
    } else if (count != 0) {
        s.arr[2] = heights[initial_cell[0]][initial_cell[1]];
        v.arr[2] = abs(v.arr[2]);
    }
}

std::array<int, 3> get_cell(Vec3 s) {
    return std::array{
        std::min(static_cast<int>(s.x() / cell_width), cell_count - 1),
        std::min(static_cast<int>(s.y() / cell_width), cell_count - 1),
        std::min(static_cast<int>(s.z() / cell_width), cell_count - 1)
    };
}

void avoidTank(const Vec3& s, const Vec3& v, Vec3& a) {
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