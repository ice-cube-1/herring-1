#include "consts.h"
#include "vec3.h"
#include "school.h"
#include "predator.h"
#include "herring.h"
#include <iostream>


Predator::Predator() {
    for (int i = 0; i<dimensions; i++) {
        s.arr[i] = (rand()%800)/80.0f;
        v.arr[i] = (rand() % 10 - 5) / 5.0f; 
        a.arr[i] = 0;
    }
}

int Predator::color() {
    return 240-s.z()*10;
}

void Predator::move(std::vector<School> schools) {
    School chosen_school = schools[0];
    float dist = std::pow(2,31)-1;
    for (School school: schools) {
        float check_dist = (school.average_s() - s).abs();
        if (dist > check_dist && school.herring.size() > 4) {
            dist = check_dist;
            chosen_school = school;
        }
    }
    attack_school(chosen_school);
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
    float scale_factor = 1;
    if (abs_v > max_v_cod) {
        scale_factor = max_v_cod / abs_v;
    } else if (abs_v < min_v_cod) {
        scale_factor = min_v_cod / abs_v;
    }
    v = v * scale_factor;
    for (int dimension = 0; dimension<dimensions; dimension++) {
        float dx = distribution(generator)*sigma + v.arr[dimension];
        s.arr[dimension] += dx*d_t;
    }
}