#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>
#include <array>
#include "vec3.h"
#include "consts.h"
#include "fish.h"

void remove_unordered(std::vector<Fish*>& vec, Fish* value) {
    std::vector<Fish *>::iterator it = std::find(vec.begin(), vec.end(), value);
    *it = std::move(vec.back());
    vec.pop_back();
}

sf::CircleShape drawFish(Fish* fish) {
    sf::CircleShape circle(4.f);
    circle.setFillColor(sf::Color{0, static_cast<sf::Uint8>(fish->color()), 255});
    circle.setPosition(fish->s.x()*60, fish->s.y()*60);
    return circle;
}

std::vector<Fish*> all_fish[cell_count][cell_count][cell_count];
std::vector<std::vector<std::array<int, 3>>> schools;
std::vector<std::array<int, 3>> current_school;
Fish fish_lst[fishCount];


bool check_cell_in_school(std::array<int, 3> to_check) {
    for (std::vector<std::array<int, 3>> school: schools) {
        for (std::array<int, 3> cell: school) {
            if (cell == to_check) {
                return true;
            }
        }
    }
    return false;
}

void flood_fill(std::array<int, 3> coords) {
    current_school.push_back(coords);
    int directions[6][3] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}};
    for (int* direction: directions) {
        std::array<int,3> to_check = {direction[0]+coords[0],direction[1]+coords[1],direction[2]+coords[2]};
        if (to_check[0] >= 0 && to_check[0] < cell_count && to_check[1] >= 0 && to_check[1] < cell_count && to_check[2] >= 0 && to_check[2] < cell_count) {
            if (std::find(current_school.begin(), current_school.end(), to_check) == current_school.end()) {
                if (all_fish[to_check[0]][to_check[1]][to_check[2]].size() != 0) {
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
                if (!check_cell_in_school(coords) && all_fish[i][j][k].size() != 0) {
                    current_school = {};
                    flood_fill(coords);
                    schools.push_back(current_school);
                }
            }
        }
    }
}


int main() {
    for (int i = 0; i<fishCount; i++) {
        fish_lst[i].assign_cell(all_fish);
    }
    sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    for (int t = 0; t<36000; t++) {
        window.clear(sf::Color::White);
        find_schools();
        std::cout << schools.size();
        for (std::vector<std::array<int, 3>> school: schools) {
            std::cout << " " << school.size() << " ";
        }
        std::cout << "\n";
        for (int i = 0; i<cell_count; i++) {
            for (int j = 0; j < cell_count; j++) {
                for (int k = 0; k< cell_count; k++) {
                    std::vector<Fish*> fish_nearby;
                    int i_min = std::max(0, i-1);
                    int i_max = std::min(cell_count-1, i+1);
                    int j_min = std::max(0, j-1);
                    int j_max = std::min(cell_count-1, j+1);
                    int k_min = std::max(0, k-1);
                    int k_max = std::min(cell_count-1, k+1);
                    for (int ci = i_min; ci <= i_max; ci++) {
                        for (int cj = j_min; cj <= j_max; cj++) {
                            for (int ck = k_min; ck <= k_max; ck++) {
                                fish_nearby.insert(fish_nearby.end(), all_fish[ci][cj][ck].begin(), all_fish[ci][cj][ck].end());
                            }
                        }
                    }

                    std::vector<Fish*> current_fishes = all_fish[i][j][k];
                    for (Fish* fish : current_fishes) {
                        fish->move(fish_nearby);
                        window.draw(drawFish(fish));
                        if (static_cast<int>(fish->s.x() / cell_width) != i ||
                            static_cast<int>(fish->s.y() / cell_width) != j ||
                            static_cast<int>(fish->s.z() / cell_width) != k) {
                            remove_unordered(all_fish[i][j][k], fish);
                            fish->assign_cell(all_fish);
                        }
                    }
                }
            }
        }
        //std::cout<<t<<"\n";
        window.display();
    }
}