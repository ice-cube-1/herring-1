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
    Predator();
    void move(std::vector<School> schools);
    private:
    void attack_school(School& school);
};

#endif