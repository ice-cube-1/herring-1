#include "consts.h"
#include "vec3.h"
#include "school.h"
#include "predator.h"
#include "herring.h"
#include <iostream>
#include <algorithm>
#include "utils.h"

Predator::Predator() {}

void Predator::create(std::default_random_engine& g, std::normal_distribution<double>& d) {
    distribution = d;
    generator = g;
    for (int i = 0; i<dimensions; i++) {
        std::uniform_real_distribution<float> dist(0.0f, tank_size);
        s.arr[i] = dist(g);
        v.arr[i] = dist(g)/10; 
        a.arr[i] = 0;
    }
    e = 1;
}

int Predator::color() {
    return 240-s.y()*10;
}

void Predator::move(const std::vector<School>& schools) {
    School chosen_school = schools[0];
    float w = std::pow(2,31)-1;
    for (const School& school: schools) {
        float dist = (school.average_s() - s).abs();
        if (dist < vision_range_cod) {
            float check_w = (school.average_s() - s).abs()*(1+k/school.herring.size());
            if (w > check_w) {
                w = check_w;
                chosen_school = school;
            }
        }
    }
    if (w == std::pow(2,31)-1) { mill(); }
    else { attack_school(chosen_school); }
}

void Predator::mill() {
    avoidTank(s,v,a);
    v = v * (0.5 * cod_body_length / v.abs()); 
    for (int dimension = 0; dimension<dimensions; dimension++) {
        float dx = distribution(generator)*sigma + v.arr[dimension];
        s.arr[dimension] += dx*d_t;
        if (s.arr[dimension]<0) {s.arr[dimension] = 0+epsilon; v.arr[dimension] *= -1; v=v*hit_wall_multiplier; }
        if (s.arr[dimension]>tank_size) {s.arr[dimension] = tank_size-epsilon; v.arr[dimension] *= -1; v=v*hit_wall_multiplier; }
        e += d_t / 60;
        if (e>1) e=1;
    }
    avoid_floor_hard(s,v);
}

void Predator::attack_school(School& school) {
    a = Vec3();
    /* STRATEGY I - GO FOR CENTRE OF SCHOOL */
    /* Vec3 s_c = school.average_s();
    Vec3 v_c = school.average_v();
    a = ((s - s_c) + (v - v_c) * gamma_2) * -gamma_1;
    a = a * (std::pow(r_2, theta_2)/std::pow((s-s_c).abs(),theta_2)); */
    /* STRATEGY II - WEIGHTED TO NEARER FISH */
    /* for (Herring* herring: school.herring) {
        Vec3 a_herring = ((s - herring->s) + (v - herring->v) * gamma_2) * -gamma_1;
        a_herring = a_herring * (std::pow(r_2, theta_2)/std::pow((s-herring->s).abs(), theta_2));
        a = a + a_herring;
    }
    a = a * ((float)1/school.herring.size()); */
    /* STRATEGY III  - NEAREST FISH IN SCHOOL ONLY */
    Herring* nearest_herring = school.herring[0];
    for (Herring* herring: school.herring) {
        float dist = 1e6f;
        float check_dist = (herring->s - s).abs();
        if (dist > check_dist) {
            dist = check_dist;
            nearest_herring = herring;
        }
    }
    a = ((s-nearest_herring->s)+(v-nearest_herring->v)*gamma_2) * -gamma_1;
    a = a * (std::pow(r_2, theta_2)/std::pow((s-nearest_herring->s).abs(),theta_2));
    float abs_a = a.abs();
    if (abs_a > max_a_cod) {
        a = a * (max_a_cod / abs_a);
    }
    v = v+a*d_t;
    float abs_v = v.abs();
    float max_v = std::clamp(0.8f+e/(school.average_s()-s).abs(), 0.5f, 5.8f)*cod_body_length;
    float scale_factor = 1;
    if (abs_v > max_v) { scale_factor = max_v / abs_v; }
    v = v * scale_factor;
    for (int dimension = 0; dimension<dimensions; dimension++) {
        float dx = distribution(generator)*sigma + v.arr[dimension];
        s.arr[dimension] += dx*d_t;
        if (s.arr[dimension]<0) {s.arr[dimension] = 0+epsilon; v.arr[dimension] *= -1; }
        if (s.arr[dimension]>tank_size) {s.arr[dimension] = tank_size-epsilon; v.arr[dimension] *= -1; }
    }
    abs_v = v.abs();
    if (abs_v <= cod_body_length) { e += d_t / 120; }
    else { e -= d_t*abs(1-std::pow(53,(abs_v/cod_body_length-1)/2.0f))/3120.0f; }
    if (e>1) { e=1; }
}