#ifndef FISH_H
#define FISH_H

#include "vec3.h"
#include "consts.h"
#include <vector>
#include <random>

class Fish {
public:
    Vec3 s;
    Vec3 v;
    Vec3 a;
    Fish();
    int color();
    void assign_cell(std::vector<Fish*> cells[cell_count][cell_count][cell_count]);
    void move(std::vector<Fish*> visible);

private:
    void avoidTank();
    void school(Fish* other_fish);
    void normalise_and_move();
    bool check_fish_visible(Fish* to_check);
};

#endif
