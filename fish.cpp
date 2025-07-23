#include "vec3.h"
#include "consts.h"
#include <iostream>
#include <random>
#include <windows.h>
#include "fish.h"

std::default_random_engine generator;
std::normal_distribution<double> distribution(0, sqrt(d_t));


Fish::Fish () { 
    for (int i = 0; i<dimensions; i++) {
        s.arr[i] = (rand()%800)/80.0f;
        v.arr[i] = (rand() % 10 - 5) / 5.0f; 
        a.arr[i] = 0;
    }
}
int Fish::color() {
    return 240-s.z()*10;
}
void Fish::assign_cell(std::vector<Fish*> cells[cell_count][cell_count][cell_count]) {
    cells[static_cast<int>(s.x() / cell_width)][static_cast<int>(s.y() / cell_width)][static_cast<int>(s.z() / cell_width)].push_back(this);
}
void Fish::move(std::vector<Fish*> visible) {
    a = {0,0,0};
    for (Fish* other_fish: visible) {
        if (check_fish_visible(other_fish)) {
            school(other_fish);
        }
    }
    avoidTank();
    normalise_and_move();
}
void Fish::avoidTank() {
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
                reflection_pos.arr[other_d] = (s.arr[dimension] * v.arr[other_d])/(reflection_pos.arr[dimension] - v.arr[dimension]) + s.arr[other_d];
            }
        }
        float abs_distance = (reflection_pos - s).abs();
        float scalar_collision = r_p/std::pow(abs_distance,p) + r_q/std::pow(abs_distance,q);
        a = a - (v - reflection_vector) * (gamma * scalar_collision);
    }
}
void Fish::school(Fish* other_fish) {
    float abs_distance = (other_fish->s - s).abs();
    float p_term = r_p/std::pow(abs_distance,p);
    float q_term = r_q/std::pow(abs_distance,q);
    float scalar_s = p_term - q_term;
    float scalar_v = p_term + q_term;
    Vec3 v_vec = v - other_fish->v;
    Vec3 s_vec = s - other_fish->s;
    a = a + s_vec * (scalar_s * alpha) - v_vec * (scalar_v * beta);
}

void Fish::normalise_and_move() {
    float abs_a = a.abs();
    if (abs_a > max_dv) {
        a = a * (max_dv / abs_a);
    }
    v = v+(a*d_t);
    float abs_v = v.abs();
    float scale_factor = 1;
    if (abs_v > max_v) {
        scale_factor = max_v / abs_v;
    } else if (abs_v < min_v) {
        scale_factor = min_v / abs_v;
    }
    v = v * scale_factor;
    for (int dimension = 0; dimension<dimensions; dimension++) {
        float dx = distribution(generator)*sigma + v.arr[dimension];
        s.arr[dimension] += dx*d_t;
    }
}

bool Fish::check_fish_visible(Fish* to_check) {
    Vec3 difference = to_check->s - s;
    float cos_theta = v.dot_product(difference) / (difference.abs() * v.abs());
    return cos_theta > cos_fov && to_check != this;
}
