#include <iostream>
//#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>
#include <array>
#include "vec3.h"
#include "consts.h"
#include "herring.h"
#include "school.h"
#include "predator.h"
#include "utils.h"
#include <cmath>
#include <algorithm>
#include <fstream>

// sf::CircleShape drawHerring(Herring* herring) {
//     sf::CircleShape circle(4.f);
//     circle.setFillColor(sf::Color{0, static_cast<sf::Uint8>(herring->color()), 255});
//     circle.setPosition(herring->s.x()*40+100, herring->s.z()*40+100);
//     return circle;
// }

bool check_cell_in_school(std::array<int, 3> to_check, std::vector<School>& schools) {
    for (School school: schools) {
        for (std::array<int, 3> cell: school.cells) {
            if (cell == to_check) {
                return true;
            }
        }
    }
    return false;
}

void flood_fill(std::array<int, 3> coords, School& current_school, std::array<std::array<std::array<std::vector<Herring*>, cell_count>, cell_count>, cell_count>& all_herring) {
    current_school.cells.push_back(coords);
    for (Herring* h : all_herring[coords[0]][coords[1]][coords[2]]) {
        current_school.herring.push_back(h);
    }
    int directions[6][3] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}};
    for (int* direction: directions) {
        std::array<int,3> to_check = {direction[0]+coords[0],direction[1]+coords[1],direction[2]+coords[2]};
        if (to_check[0] >= 0 && to_check[0] < cell_count && to_check[1] >= 0 && to_check[1] < cell_count && to_check[2] >= 0 && to_check[2] < cell_count) {
            if (std::find(current_school.cells.begin(), current_school.cells.end(), to_check) == current_school.cells.end()) {
                if (all_herring[to_check[0]][to_check[1]][to_check[2]].size() != 0) {
                    flood_fill((std::array<int,3>){to_check}, current_school, all_herring);
                }
            }
        }
    }
}

void find_schools(std::array<std::array<std::array<std::vector<Herring*>, cell_count>, cell_count>, cell_count>& all_herring, std::vector<School>& schools) {
    int size = schools.size();
    schools.clear();
    for (int i = 0; i<cell_count; i++) {
        for (int j = 0; j<cell_count; j++) {
            for (int k = 0; k<cell_count; k++) {
                std::array<int,3> coords = {i,j,k};
                if (!check_cell_in_school(coords, schools) && all_herring[i][j][k].size() > 0) {
                    School current_school;
                    flood_fill(coords, current_school, all_herring);
                    schools.push_back(std::move(current_school));
                }
            }
        }
    }
}

// sf::CircleShape drawPredator(Predator predator) {
//     sf::CircleShape circle(6.f);
//     circle.setFillColor(sf::Color{255,static_cast<sf::Uint8>(predator.color()),0});
//     circle.setPosition(predator.s.x()*40+100, predator.s.z()*40+100);
//     return circle;
// }

int run_sim(int seed) {
    srand(seed);
    generator.seed(seed);
    std::array<std::array<std::array<std::vector<Herring*>, cell_count>, cell_count>, cell_count> all_herring;
    std::vector<School> schools;
    Herring herring_lst[herringCount];
    Predator predators[predator_count];
    for (int i = 0; i<herringCount; i++) {
        herring_lst[i].assign_cell(all_herring);
    }
    int alive = herringCount;
    //sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    for (int t = 0; t<3*60*10; t++) {
        //window.clear(sf::Color::White);
        find_schools(all_herring, schools);
        for (int i = 0; i<predator_count; i++) {
            predators[i].move(schools);
            //window.draw(drawPredator(predators[i]));
        }
        for (int i = 0; i<cell_count; i++) {
            for (int j = 0; j < cell_count; j++) {
                for (int k = 0; k< cell_count; k++) {
                    std::vector<Herring*> herring_nearby;
                    int i_min = std::max(0, i-1);
                    int i_max = std::min(cell_count-1, i+1);
                    int j_min = std::max(0, j-1);
                    int j_max = std::min(cell_count-1, j+1);
                    int k_min = std::max(0, k-1);
                    int k_max = std::min(cell_count-1, k+1);
                    for (int ci = i_min; ci <= i_max; ci++) {
                        for (int cj = j_min; cj <= j_max; cj++) {
                            for (int ck = k_min; ck <= k_max; ck++) {
                                herring_nearby.insert(herring_nearby.end(), all_herring[ci][cj][ck].begin(), all_herring[ci][cj][ck].end());
                            }
                        }
                    }

                    std::vector<Herring*> current_herrings = all_herring[i][j][k];
                    for (Herring* herring : current_herrings) {
                        if (!herring->move(herring_nearby, predators)) {
                            remove_unordered(all_herring[i][j][k], herring);
                            alive -= 1;
                        } else {
                            //window.draw(drawHerring(herring));
                            if (get_cell(herring->s)!=std::array{i,j,k}) {
                                remove_unordered(all_herring[i][j][k], herring);
                                herring->assign_cell(all_herring);
                            }
                        }
                    }
                }
            }
        }
        //window.display();
    }    
    std::cout<<"Alive:"<<alive<<"\n";
    return alive;
}

constexpr int dim = 4;
constexpr int pop_size = 10;
const int max_gen = 10;
const double F = 0.8;
const double CR = 0.9;

class Param {
    public:
    std::string name;
    float lower_bound;
    float upper_bound;
    Param(std::string n, float l, float u) {name = n, lower_bound = l; upper_bound = u; }
};

const std::array<Param, dim> params = {Param("α",0, 1.5), Param("β",0,2), Param("γ",0,4), Param("δ",1,2)};

void print_arr(const std::array<double,dim>&x) {
    for (int i = 0; i<dim; i++) {
        std::cout<<params[i].name<<": "<<x[i]<<"  ";
    }
    std::cout<<"\n";
}

double objective(const std::array<double,dim>& x) {
    print_arr(x);
    set_params(x);
    double val = run_sim(5);
    std::ofstream file("output.csv", std::ios::app);
    file<<x[0]<<","<<x[1]<<","<<x[2]<<","<<x[3]<<","<<val<<"\n";
    file.close();
    srand(time(nullptr));
    return val;
}

double rand_double(double min, double max) {
    return min + (max - min) * (rand() / (double)RAND_MAX);
}


class Sample {
    public:
    std::array<double,dim> vals;
    double fitness;
    Sample() {
        for (int i = 0; i<dim; i++) {
            vals[i] = rand_double(params[i].lower_bound, params[i].upper_bound);
        }
        fitness = objective(vals);
    }
    Sample(std::array<double,dim> v, double f) {
        vals = v;
        fitness = f;
    }
};

int main() {
    init_planes();
    std::ofstream file("output.csv");
    file<<"alpha,beta,gamma,delta,alive\n";
    file.close();
    srand(time(nullptr));
    std::array<Sample,pop_size> population;
    for (int gen = 0; gen < max_gen; gen++) {
        for (int i = 0; i < pop_size; i++) {
            int a, b, c;
            do { a = rand() % pop_size; } while (a == i);
            do { b = rand() % pop_size; } while (b == i || b == a);
            do { c = rand() % pop_size; } while (c == i || c == a || c == b);
            std::array<double,dim> trial;
            int rand_j = rand() % dim;
            for (int j = 0; j < dim; j++) {
                if ((rand_double(0, 1) < CR) || (j == rand_j)) {
                    trial[j] = population[a].vals[j] + F * (population[b].vals[j] - population[c].vals[j]);
                    if (trial[j] < params[j].lower_bound) trial[j] = params[j].lower_bound;
                    if (trial[j] > params[j].upper_bound) trial[j] = params[j].upper_bound;
                } else {
                    trial[j] = population[i].vals[j];
                }
            }
            double trial_fitness = objective(trial);
            if (trial_fitness > population[i].fitness) {
                std::cout<<"New point! "<<trial_fitness<<" - "<<population[i].fitness<<"\n";
                population[i] = Sample(trial, trial_fitness);
            }
        }
    }
    int best_index = 0;
    for (int i = 1; i < pop_size; i++)
        if (population[i].fitness < population[best_index].fitness)
            best_index = i;

    std::cout << "Best solution found:\n";
    print_arr(population[best_index].vals);
}