#ifndef herring_H
#define herring_H

#include "vec3.h"
#include "consts.h"
#include "predator.h"
#include <vector>
#include <random>

class Herring {
public:
    Vec3 s;
    Vec3 v;
    Vec3 a;
    Herring();
    void create(std::default_random_engine& generator, std::normal_distribution<double>& distribution);
    int color();
    void assign_cell(std::array<std::array<std::array<std::vector<Herring*>, 10>, 10>, 10>& cells);
    bool move(std::vector<Herring*> visible, Predator* predators);

private:
    Vec3 old_s;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
    void school(Herring* other_herring);
    void normalise_and_move();
    bool check_herring_visible(Herring* to_check);
    bool avoid_predators(Predator predators[]);
};

#endif
