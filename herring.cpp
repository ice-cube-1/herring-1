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
        s.arr[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/15));
        v.arr[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
        a.arr[i] = 0;
    }
}
int Herring::color() {
    return 240-s.z()*10;
}
void Herring::assign_cell(std::vector<Herring*> cells[cell_count][cell_count][cell_count]) {
    int ix = std::min(static_cast<int>(s.x() / cell_width), cell_count - 1);
    int iy = std::min(static_cast<int>(s.y() / cell_width), cell_count - 1);
    int iz = std::min(static_cast<int>(s.z() / cell_width), cell_count - 1);
    cells[ix][iy][iz].push_back(this);
}
bool Herring::move(std::vector<Herring*> visible, Predator* predators) {
    a = {0,0,0};
    for (Herring* other_herring: visible) {
        if (check_herring_visible(other_herring)) {
            school(other_herring);
        }
    }
    avoidTank();
    if (!avoid_predators(predators)) {
        return false;
    }
    normalise_and_move();
    return true;
}
void Herring::avoidTank() {
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
                if (abs(divisor) < 1e-6f) { divisor = 1e-6f; }
                reflection_pos.arr[other_d] = (s.arr[dimension] * v.arr[other_d])/divisor + s.arr[other_d];
            }
        }
        float abs_distance = (reflection_pos - s).abs();
        if (abs_distance < 1e-6f) {
            abs_distance = 1e-6f;
        }
        float scalar_collision = r_p/std::pow(abs_distance,p) + r_q/std::pow(abs_distance,q);
        if (std::isfinite(scalar_collision)) {
            a = a - (v - reflection_vector) * (gamma * scalar_collision);
        }
    }
}
void Herring::school(Herring* other_herring) {
    const float dist_eps = 1e-6f;
    float abs_distance = (other_herring->s - s).abs();
    abs_distance = std::max(abs_distance, dist_eps);
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
        a = a + (s-predators[i].s) * (delta * std::pow(r_1, theta_1)/std::max(1e-6f,static_cast<float>(std::pow((s-predators[i].s).abs(), theta_1))));
    } 
    return true;
}

void Herring::normalise_and_move() {
    float abs_a = std::max(1e-6f,a.abs());
    if (abs_a > max_dv) {
        a = a * (max_dv / abs_a);
    }
    v = v+(a*d_t);
    float abs_v = std::max(1e-6f,v.abs());
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
        if (s.arr[dimension] < 0) { s.arr[dimension] = 0; }
        if (s.arr[dimension] > tank_size) { s.arr[dimension] = tank_size; }
    }
}

bool Herring::check_herring_visible(Herring* to_check) {
    Vec3 difference = to_check->s - s;
    float cos_theta = v.dot_product(difference) / (difference.abs() * v.abs());
    return cos_theta > cos_fov && to_check != this;
}
