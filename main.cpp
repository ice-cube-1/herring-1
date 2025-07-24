#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>
#include <array>
#include "vec3.h"
#include "consts.h"
#include "herring.h"

void remove_unordered(std::vector<Herring*>& vec, Herring* value) {
    std::vector<Herring *>::iterator it = std::find(vec.begin(), vec.end(), value);
    *it = std::move(vec.back());
    vec.pop_back();
}

sf::CircleShape drawHerring(Herring* herring) {
    sf::CircleShape circle(4.f);
    circle.setFillColor(sf::Color{0, static_cast<sf::Uint8>(herring->color()), 255});
    circle.setPosition(herring->s.x()*60, herring->s.y()*60);
    return circle;
}

class School {
    public:
    std::vector<std::array<int, 3>> cells;
    std::vector<Herring*> herring;
    School() {};
    Vec3 average_s() {
        Vec3 s;
        for (Herring* h: herring) { s = s + h->s; }
        return s * (1.0f / herring.size());
    }
    Vec3 average_v() {
        Vec3 v;
        for (Herring* h: herring) { v = v + h->v; }
        return v * (1.0f / herring.size());
    }
};

std::vector<Herring*> all_herring[cell_count][cell_count][cell_count];
std::vector<School> schools;
School current_school;
Herring herring_lst[herringCount];


bool check_cell_in_school(std::array<int, 3> to_check) {
    for (School school: schools) {
        for (std::array<int, 3> cell: school.cells) {
            if (cell == to_check) {
                return true;
            }
        }
    }
    return false;
}

void flood_fill(std::array<int, 3> coords) {
    current_school.cells.push_back(coords);
    current_school.herring.insert(current_school.herring.end(), all_herring[coords[0]][coords[1]][coords[2]].begin(), all_herring[coords[0]][coords[1]][coords[2]].end() );
    int directions[6][3] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}};
    for (int* direction: directions) {
        std::array<int,3> to_check = {direction[0]+coords[0],direction[1]+coords[1],direction[2]+coords[2]};
        if (to_check[0] >= 0 && to_check[0] < cell_count && to_check[1] >= 0 && to_check[1] < cell_count && to_check[2] >= 0 && to_check[2] < cell_count) {
            if (std::find(current_school.cells.begin(), current_school.cells.end(), to_check) == current_school.cells.end()) {
                if (all_herring[to_check[0]][to_check[1]][to_check[2]].size() != 0) {
                    flood_fill((std::array<int,3>){to_check});
                }
            }
        }
    }
}

void find_schools() {
    schools = {};
    for (int i = 0; i<cell_count; i++) {
        for (int j = 0; j<cell_count; j++) {
            for (int k = 0; k<cell_count; k++) {
                std::array<int,3> coords = {i,j,k};
                if (!check_cell_in_school(coords) && all_herring[i][j][k].size() > 0) {
                    current_school = {};
                    flood_fill(coords);
                    schools.push_back(current_school);
                }
            }
        }
    }
}

class Predator {
    public:
    Vec3 s;
    Vec3 v;
    Vec3 a;
    Predator() {
        for (int i = 0; i<dimensions; i++) {
            s.arr[i] = (rand()%800)/80.0f;
            v.arr[i] = (rand() % 10 - 5) / 5.0f; 
            a.arr[i] = 0;
        }
    }
    void move(std::vector<School> schools) {
        School chosen_school = schools[0];
        float dist = std::pow(2,31)-1;
        for (School school: schools) {
            float check_dist = (school.average_s() - s).abs();
            if (dist > check_dist) {
                dist = check_dist;
                chosen_school = school;
            }
        }
        attack_school(chosen_school);
    }
    private:
    void attack_school(School& school) {
        Vec3 s_c = school.average_s();
        Vec3 v_c = school.average_v();
        a = ((s - s_c) + (v - v_c) * gamma_2) * -gamma_1;
        a = a * (std::pow(r_2, theta_2)/std::pow((s-s_c).abs(),theta_2));
        std::cout << a.abs()<<" a ";
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
};

sf::CircleShape drawPredator(Predator predator) {
    sf::CircleShape circle(6.f);
    circle.setFillColor(sf::Color{255,0,0});
    circle.setPosition(predator.s.x()*60, predator.s.y()*60);
    return circle;
}

Predator predators[5];
int main() {
    for (int i = 0; i<herringCount; i++) {
        herring_lst[i].assign_cell(all_herring);
    }
    sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    for (int t = 0; t<36000; t++) {
        window.clear(sf::Color::White);
        find_schools();
        for (int i = 0; i<5; i++) {
            predators[i].move(schools);
            window.draw(drawPredator(predators[i]));
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

                    std::vector<Herring*> current_herringes = all_herring[i][j][k];
                    for (Herring* herring : current_herringes) {
                        herring->move(herring_nearby);
                        window.draw(drawHerring(herring));
                        if (static_cast<int>(herring->s.x() / cell_width) != i ||
                            static_cast<int>(herring->s.y() / cell_width) != j ||
                            static_cast<int>(herring->s.z() / cell_width) != k) {
                            remove_unordered(all_herring[i][j][k], herring);
                            herring->assign_cell(all_herring);
                        }
                    }
                }
            }
        }
        //std::cout<<t<<"\n";
        window.display();
    }
}