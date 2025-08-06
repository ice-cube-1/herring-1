#ifndef PREDATOR_H
#define PREDATOR_H

#include "vec3.h"
#include "consts.h"

class School;

class Predator {
    public:
    Vec3 s;
    Vec3 v;
    Vec3 a;
    float e;
    Predator();
    int color();
    void move(const std::vector<School>& schools);
    private:
    void attack_school(School& school);
    void mill();
    void create(std::default_random_engine& generator, std::normal_distribution<double>& distribution);
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
};

#endif