#include "vec3.h"
#include "consts.h"
#include <iostream>
#include <random>
#include <windows.h>
#include "herring.h"
#include "predator.h"
#include "utils.h"


Herring::Herring () { 
    for (int i = 0; i<dimensions; i++) {
        s.arr[i] = 5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (10.0f));
        v.arr[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
        a.arr[i] = 0;
    }
}
int Herring::color() {
    return 240-s.y()*10;
}
void Herring::assign_cell(std::vector<Herring*> cells[cell_count][cell_count][cell_count]) {
    std::array<int, 3> cell = get_cell(s);
    cells[cell[0]][cell[1]][cell[2]].push_back(this);
}

bool Herring::move(std::vector<Herring*> visible, Predator* predators) {
    a = {0,0,0};
    for (Herring* other_herring: visible) {
        if (check_herring_visible(other_herring)) {
            school(other_herring);
        }
    }
    avoidTank(s,v,a);
    if (!avoid_predators(predators)) {
        return false;
    }
    normalise_and_move();
    return true;
}

void Herring::school(Herring* other_herring) {
    float abs_distance = (other_herring->s - s).abs();
    if (abs_distance > vision_range_herring) return;
    abs_distance = std::max(abs_distance, epsilon);
    float p_term = r_p / std::pow(abs_distance, p);
    float q_term = r_q / std::pow(abs_distance, q);
    float scalar_s = p_term - q_term;
    float scalar_v = p_term + q_term;
    Vec3 v_vec = v - other_herring->v;
    Vec3 s_vec = s - other_herring->s;
    a = a + s_vec * (scalar_s * alpha) - v_vec * (scalar_v * beta);
}

bool Herring::avoid_predators(Predator predators[]) {
    for (int i = 0; i<predator_count; i++) {
        if ((s-predators[i].s).abs() < 0.2) {
            return false;
        }
        a = a + (s-predators[i].s) * (delta * std::pow(r_1, theta_1)/std::max(epsilon,static_cast<float>(std::pow((s-predators[i].s).abs(), theta_1))));
    } 
    return true;
}

void Herring::normalise_and_move() {
    float abs_a = std::max(epsilon,a.abs());
    if (abs_a > max_dv) {
        a = a * (max_dv / abs_a);
    }
    v = v+(a*d_t);
    float abs_v = std::max(epsilon,v.abs());
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
        if (s.arr[dimension]<0) {s.arr[dimension] = 0; v.arr[dimension] *= -1; }
        if (s.arr[dimension]>tank_size) {s.arr[dimension] = tank_size; v.arr[dimension] *= -1; }
    }
    avoid_floor_hard(s,v);
}

bool Herring::check_herring_visible(Herring* to_check) {
    Vec3 difference = to_check->s - s;
    float cos_theta = v.dot_product(difference) / (difference.abs() * v.abs());
    return cos_theta > cos_fov && to_check != this && difference.abs() < vision_range_herring;
}
