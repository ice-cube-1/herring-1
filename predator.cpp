#include "consts.h"
#include "vec3.h"
#include "school.h"
#include "predator.h"
#include "herring.h"
#include <iostream>
#include <algorithm>


Predator::Predator() {
    for (int i = 0; i<dimensions; i++) {
        s.arr[i] = (rand()%800)/80.0f;
        v.arr[i] = (rand() % 10 - 5) / 5.0f; 
        a.arr[i] = 0;
    }
    e = 1;
}

int Predator::color() {
    return 240-s.z()*10;
}

void Predator::move(const std::vector<School>& schools) {
    School chosen_school = schools[0];
    float dist = std::pow(2,31)-1;
    for (const School& school: schools) {
        float check_dist = (school.average_s() - s).abs();
        if (dist > check_dist) {
            dist = check_dist;
            chosen_school = school;
        }
    }
    if (dist > fov_cod) { mill(); }
    else { attack_school(chosen_school); }
}

void Predator::mill() {
    for (int dimension = 0; dimension<dimensions; dimension++) {
        float abs_v = v.abs();
        float scale_factor = 1;
        if (abs_v > 0.8 * cod_body_length) { scale_factor = 0.8 * cod_body_length / abs_v; }
        float dx = distribution(generator)*sigma + v.arr[dimension];
        s.arr[dimension] += dx*d_t;
        e -= d_t / 60;
    }
}

void Predator::attack_school(School& school) {
    /* STRATEGY 1 - GO FOR CENTRE OF SCHOOL */
    /* Vec3 s_c = school.average_s();
    Vec3 v_c = school.average_v();
    a = ((s - s_c) + (v - v_c) * gamma_2) * -gamma_1;
    a = a * (std::pow(r_2, theta_2)/std::pow((s-s_c).abs(),theta_2)); */
    /* STRATEGY 2 - WEIGHTED TO NEARER FISH */
    a = Vec3();
    for (Herring* herring: school.herring) {
        Vec3 a_herring = ((s - herring->s) + (v - herring->v) * gamma_2) * -gamma_1;
        a_herring = a_herring * (std::pow(r_2, theta_2)/std::pow((s-herring->s).abs(), theta_2));
        a = a + a_herring;
    }
    a = a * ((float)1/school.herring.size());
    float abs_a = a.abs();
    if (abs_a > max_a_cod) {
        a = a * (max_a_cod / abs_a);
    }
    v = v+a*d_t;
    float abs_v = v.abs();
    float max_v = std::min(5.8f, 1+e/(school.average_s().abs()))*cod_body_length;
    float scale_factor = 1;
    if (abs_v > max_v) { scale_factor = max_v / abs_v; }
    v = v * scale_factor;
    for (int dimension = 0; dimension<dimensions; dimension++) {
        float dx = distribution(generator)*sigma + v.arr[dimension];
        s.arr[dimension] += dx*d_t;
    }
    abs_v = v.abs();
    if (abs_v < 1) { e -= d_t / 120; }
    else { e += (1-std::pow(53,(abs_v-1)/2))/3120; }
}