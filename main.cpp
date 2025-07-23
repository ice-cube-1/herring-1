#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>
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
Fish fish_lst[fishCount];
int main() {
    for (int i = 0; i<fishCount; i++) {
        fish_lst[i].assign_cell(all_fish);
    }
    sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    for (int t = 0; t<36000; t++) {
        window.clear(sf::Color::White);
        for (int i = 0; i<cell_count; i++) {
            for (int j = 0; j < cell_count; j++) {
                for (int k = 0; k< cell_count; k++) {
                    std::vector<Fish*> fish_nearby;
                    for (int ci = i-1; ci <= i+1; ci++) {
                        if (0 <= ci && ci < cell_count) {
                            for (int cj = j-1; cj <= j+1; cj++) {
                                if (0 <= cj && cj < cell_count) {
                                    for (int ck = k-1; ck <= k+1; ck++) {
                                        if (0 <= ck && ck < cell_count) {
                                            fish_nearby.insert(fish_nearby.end(), all_fish[ci][cj][ck].begin(), all_fish[ci][cj][ck].end());
                                        }
                                    }
                                }
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
        std::cout<<t<<"\n";
        window.display();
    }
}