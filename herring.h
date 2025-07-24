#ifndef herring_H
#define herring_H

#include "vec3.h"
#include "consts.h"
#include <vector>
#include <random>

class Herring {
public:
    Vec3 s;
    Vec3 v;
    Vec3 a;
    Herring();
    int color();
    void assign_cell(std::vector<Herring*> cells[cell_count][cell_count][cell_count]);
    void move(std::vector<Herring*> visible);

private:
    void avoidTank();
    void school(Herring* other_herring);
    void normalise_and_move();
    bool check_herring_visible(Herring* to_check);
};

#endif
