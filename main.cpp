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
#include <future>
#include <string>

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
    std::default_random_engine generator;
    generator.seed(seed);
    std::normal_distribution<double> distribution(0, std::sqrt(d_t));
    std::array<std::array<std::array<std::vector<Herring*>, cell_count>, cell_count>, cell_count> all_herring;
    std::vector<School> schools;
    Herring herring_lst[herringCount];
    Predator predators[predator_count];
    for (int i = 0; i<predator_count; i++) {
        predators[i].create(generator, distribution);
    }
    for (int i = 0; i<herringCount; i++) {
        herring_lst[i].create(generator, distribution);
        herring_lst[i].assign_cell(all_herring);
    }
    int alive = herringCount;
    //sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    for (int t = 0; t<7*60*10; t++) {
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

constexpr int dim_prey = 4;
constexpr int dim_predator = 3;
constexpr int pop_size = 12;
const int max_gen = 12;
const double F = 0.8;
const double CR = 0.9;
int seed_count = 16;
int std_penalizer = 0.5;

class Param {
    public:
    std::string name;
    float lower_bound;
    float upper_bound;
    float default_value;
    Param(std::string n, float l, float u, float d) {name = n, lower_bound = l; upper_bound = u; default_value = d; }
};

std::array<Param, dim_prey> params_prey = {Param("α",-3, 3, 0.2), Param("β",-3,3,0.5), Param("γ",0,4,0.5), Param("δ",1e-12,1e-3,1e-6)};
std::array<Param, dim_predator> params_predator = {Param("γ1",0, 3, 2), Param("γ2",0,3,0.5), Param("k",-1,1,0.5)};

void print_arr(const std::array<double,dim_prey>&x, const std::array<double,dim_predator>&y) {
    for (int i = 0; i<dim_prey; i++) {
        std::cout<<params_prey[i].name<<": "<<x[i]<<"  ";
    }
    for (int i = 0; i<dim_predator; i++) {
        std::cout<<params_predator[i].name<<": "<<y[i]<<"  ";
    }
    std::cout<<"\n";
}

double objective(const std::array<double,dim_prey>& prey_params, const std::array<double,dim_predator>& predator_params, std::string filename,bool min) {
    print_arr(prey_params, predator_params);
    set_prey_params(prey_params);
    set_predator_params(predator_params);
    std::vector<std::future<int>> futures;
    std::vector<int> results;
    for (int i = 0; i < seed_count; i++) {
        futures.push_back(std::async(std::launch::async, run_sim, i));
    }
    for (auto& fut : futures) {
        results.push_back(fut.get());
    }
    int total_sum = std::accumulate(results.begin(), results.end(), 0);
    double mean = static_cast<double>(total_sum) / seed_count;
    double sq_sum = 0.0;
    for (int num : results) {
        sq_sum += std::pow(num - mean, 2);
    }
    double stddev = std::sqrt(sq_sum / seed_count);
    std::ofstream file(filename, std::ios::app);
    file<<prey_params[0]<<","<<prey_params[1]<<","<<prey_params[2]<<","<<prey_params[3]<<","
    <<predator_params[0]<<","<<predator_params[1]<<","<<predator_params[2]<<","<<total_sum<<","<<stddev<<"\n";
    file.close();
    if (min) return total_sum+stddev*std_penalizer;
    return total_sum-stddev*std_penalizer;
}

double rand_double(double min, double max) {
    return min + (max - min) * (rand() / (double)RAND_MAX);
}


class Sample {
    public:
    std::array<double, dim_prey> prey_vals;
    std::array<double, dim_predator> predator_vals;
    double fitness;
    Sample() {}
    Sample(std::array<double, dim_prey> prey, std::array<double, dim_predator> predator, double f) {
        prey_vals = prey;
        predator_vals = predator;
        fitness = f;
    }
};

void optimise_prey(int iteration) {
    std::string filename = "prey_output_"+std::to_string(iteration)+".csv";
    std::ofstream file(filename);
    file<<"alpha,beta,gamma,delta,gamma1,gamma2,k,alive,stddev\n";
    file.close();
    srand(time(nullptr));
    std::array<Sample,pop_size> population;
    std::array<double, dim_predator> predator_vals;
    for (int i = 0; i<dim_predator; i++) {
        predator_vals[i] = params_predator[i].default_value;
    }
    for (int j = 0; j<pop_size; j++) {
        for (int i = 0; i<dim_prey; i++) {
            population[j].prey_vals[i] = rand_double(params_prey[i].lower_bound, params_prey[i].upper_bound);
        }
        population[j].fitness = objective(population[j].prey_vals, predator_vals,filename,false);
    }

    for (int gen = 0; gen < max_gen; gen++) {
        for (int i = 0; i < pop_size; i++) {
            int a, b, c;
            do { a = rand() % pop_size; } while (a == i);
            do { b = rand() % pop_size; } while (b == i || b == a);
            do { c = rand() % pop_size; } while (c == i || c == a || c == b);
            std::array<double,dim_prey> trial;
            int rand_j = rand() % dim_prey;
            for (int j = 0; j < dim_prey; j++) {
                if ((rand_double(0, 1) < CR) || (j == rand_j)) {
                    trial[j] = population[a].prey_vals[j] + F * (population[b].prey_vals[j] - population[c].prey_vals[j]);
                    if (trial[j] < params_prey[j].lower_bound) trial[j] = params_prey[j].lower_bound;
                    if (trial[j] > params_prey[j].upper_bound) trial[j] = params_prey[j].upper_bound;
                } else {
                    trial[j] = population[i].prey_vals[j];
                }
            }
            double trial_fitness = objective(trial, predator_vals, filename,false);
            std::cout<<"Total: "<<trial_fitness<<"\n";
            if (trial_fitness > population[i].fitness) {
                std::cout<<"New point! "<<trial_fitness<<" - "<<population[i].fitness<<"\n";
                population[i] = Sample(trial, predator_vals, trial_fitness);
            }
        }
    }
    int best_index = 0;
    for (int i = 1; i < pop_size; i++)
        if (population[i].fitness > population[best_index].fitness)
            best_index = i;
    for (int i = 0; i<dim_prey; i++) {
        params_prey[i].default_value = population[best_index].prey_vals[i];
        std::cout<<"setting "<<params_prey[i].name<<" to "<<params_prey[i].default_value<<"\n";
    }
}


void optimise_predator(int iteration) {
    std::string filename = "predator_output_"+std::to_string(iteration)+".csv";
    std::ofstream file(filename);
    file<<"alpha,beta,gamma,delta,gamma1,gamma2,k,alive,stddev\n";
    file.close();
    srand(time(nullptr));
    std::array<Sample,pop_size> population;
    std::array<double, dim_prey> prey_vals;
    for (int i = 0; i<dim_prey; i++) {
        prey_vals[i] = params_prey[i].default_value;
    }
    for (int j = 0; j<pop_size; j++) {
        for (int i = 0; i<dim_predator; i++) {
            population[j].predator_vals[i] = rand_double(params_predator[i].lower_bound, params_predator[i].upper_bound);
        }
        population[j].fitness = objective(prey_vals, population[j].predator_vals,filename,true);
    }

    for (int gen = 0; gen < max_gen; gen++) {
        for (int i = 0; i < pop_size; i++) {
            int a, b, c;
            do { a = rand() % pop_size; } while (a == i);
            do { b = rand() % pop_size; } while (b == i || b == a);
            do { c = rand() % pop_size; } while (c == i || c == a || c == b);
            std::array<double,dim_predator> trial;
            int rand_j = rand() % dim_predator;
            for (int j = 0; j < dim_predator; j++) {
                if ((rand_double(0, 1) < CR) || (j == rand_j)) {
                    trial[j] = population[a].predator_vals[j] + F * (population[b].predator_vals[j] - population[c].predator_vals[j]);
                    if (trial[j] < params_predator[j].lower_bound) trial[j] = params_predator[j].lower_bound;
                    if (trial[j] > params_predator[j].upper_bound) trial[j] = params_predator[j].upper_bound;
                } else {
                    trial[j] = population[i].predator_vals[j];
                }
            }
            double trial_fitness = objective(prey_vals, trial, filename,true);
            std::cout<<"Total: "<<trial_fitness<<"\n";
            if (trial_fitness < population[i].fitness) {
                std::cout<<"New point! "<<trial_fitness<<" - "<<population[i].fitness<<"\n";
                population[i] = Sample(prey_vals, trial, trial_fitness);
            }
        }
    }
    int best_index = 0;
    for (int i = 1; i < pop_size; i++)
        if (population[i].fitness < population[best_index].fitness)
            best_index = i;
    for (int i = 0; i<dim_predator; i++) {
        params_predator[i].default_value = population[best_index].predator_vals[i];
        std::cout<<"setting "<<params_predator[i].name<<" to "<<params_predator[i].default_value<<"\n";
    }
}

// int main() {
//     for (int i = 0; i<5; i++) {
//         optimise_prey(i);    
//         optimise_predator(i);
//     }
// }

int main() {
    std::array<double, dim_prey> prey_vals;
    for (int i = 0; i<dim_prey; i++) {
        prey_vals[i] = params_prey[i].default_value;
    }
    std::array<double, dim_predator> predator_vals;
    for (int i = 0; i<dim_predator; i++) {
        predator_vals[i] = params_predator[i].default_value;
    }
    for (int i = 0; i<dim_prey; i++) {
        for (float j = 0; j<=10; j++) {
            std::array<double, dim_prey> trial = prey_vals;
            trial[i] = (params_prey[i].upper_bound-params_prey[i].lower_bound)/9.0f*j + params_prey[i].lower_bound;
            objective(trial, predator_vals, "test_const_prey.csv",false);
        }
    }
    for (int i = 0; i<dim_predator; i++) {
        for (float j = params_predator[i].lower_bound; j<= params_predator[i].upper_bound; j+=(params_predator[i].upper_bound-params_predator[i].lower_bound)/9.0f) {
            std::array<double, dim_predator> trial = predator_vals;
            trial[i] = j;
            objective(prey_vals, trial, "test_const_predator.csv",true);
        }
    }
}