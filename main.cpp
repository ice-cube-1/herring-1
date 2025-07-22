#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>

#define d_t 0.1
#define fishCount 500

#define sigma 0.2
#define alpha 0.05
#define beta 0.1
#define gamma 0.5
#define r 1.5  
#define p 2.5    
#define q 1.5       
#define max_dv 1.5 * d_t
#define max_v 0.6
#define min_v 0.03
#define tank_size 10

#define dimensions 3

std::default_random_engine generator;
std::normal_distribution<double> distribution(0, sqrt(d_t));

float get_abs_difference(float a[dimensions], float b[dimensions]) {
    float ans = 0;
    for (int i = 0; i<dimensions; i++) {
        ans += std::pow(a[i]-b[i],2);
    }
    return std::pow(ans, 0.5);
}

float vector_abs(float a[dimensions]) {
    float ans = 0;
    for (int i = 0; i<dimensions; i++) {
        ans += std::pow(a[i],2);
    }
    return std::pow(ans, 0.5);
}

void get_vector_difference(const float a[dimensions], const float b[dimensions], float result[dimensions]) {
    for (int i = 0; i < dimensions; i++) {
        result[i] = a[i] - b[i];
    }
}

class Fish {
    public:
    float x[dimensions];
    float v[dimensions];
    Fish () { 
        for (int i = 0; i<dimensions; i++) {
            x[i] = (rand()%800)/80.0f;
            v[i] = (rand() % 10 - 5) / 5.0f; 
        }
    }
    sf::Uint8 color_alpha() {
        return x[2]*10;
    }
    void avoidTank(float dv[dimensions]) {
        for (int dimension = 0; dimension<dimensions; dimension ++) {
            float reflection_vector[dimensions];
            for (int d = 0; d < dimensions; d++) {
                if (d == dimension) { reflection_vector[d] = -v[d]; }
                else { reflection_vector[d] = v[d]; }
            }
            float reflection_pos[dimensions];
            if (v[dimension] < 0) {
                reflection_pos[dimension] = 0;
            } else {
                reflection_pos[dimension] = tank_size;
            }
            for (int other_d = 0; other_d < dimensions; other_d ++) {
                if (other_d != dimension) {
                    reflection_pos[other_d] = (x[dimension] * v[other_d])/(reflection_pos[dimension] - v[dimension]) + x[other_d];
                }
            }
            float abs_distance = get_abs_difference(reflection_pos, x);
            float scalar_collision = std::pow(r,p)/std::pow(abs_distance,p) + std::pow(r,q)/std::pow(abs_distance,q);
            for (int d = 0; d < dimensions; d++) {
                dv[d] -= gamma * scalar_collision * (v[d] - reflection_vector[d]);
            }
        }
    }
    void normalise_and_move(float dv[dimensions]) {
        float abs_dv = vector_abs(dv);
        if (abs_dv > max_dv) {
            float scale_factor = max_dv / abs_dv;
            for (int dimension = 0; dimension < dimensions; dimension++) {
                dv[dimension] *= scale_factor;
            }
        }

        for (int dimension = 0; dimension<dimensions; dimension++) {
            v[dimension] += dv[dimension]*d_t;
        }

        float abs_v = vector_abs(v);
        float scale_factor = 1;
        if (abs_v > max_v) {
            scale_factor = max_v / abs_v;
        } else if (abs_v < min_v) {
            scale_factor = min_v / abs_v;
        }
        for (int dimension = 0; dimension < dimensions; dimension++) {
            v[dimension] *= scale_factor;
        }

        for (int dimension = 0; dimension<dimensions; dimension++) {
            float dx = distribution(generator)*sigma + v[dimension];
            x[dimension] += dx*d_t;
        }
    }

};

sf::CircleShape drawFish(Fish fish) {
    sf::CircleShape circle(2.f);
    circle.setFillColor(sf::Color{0, 150, 255, fish.color_alpha()});
    circle.setPosition(fish.x[0]*80, fish.x[1]*80);
    return circle;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    Fish fishes[fishCount];
    for (int t = 0; t<100000; t++) {
        window.clear(sf::Color::White);
        for (int i = 0; i<fishCount; i ++) {
            float dv[dimensions] = {0,0,0};
            for (int j = 0; j < fishCount; j++) {
                if (j != i) {
                    float abs_distance = get_abs_difference(fishes[j].x, fishes[i].x);
                    float scalar_s = std::pow(r,p)/std::pow(abs_distance,p) - std::pow(r,q)/std::pow(abs_distance,q);
                    float scalar_v = std::pow(r,p)/std::pow(abs_distance,p) + std::pow(r,q)/std::pow(abs_distance,q);
                    float v_vec[dimensions];
                    float s_vec[dimensions];
                    get_vector_difference(fishes[i].v, fishes[j].v, v_vec);
                    get_vector_difference(fishes[i].x, fishes[j].x, s_vec);
                    for (int dimension = 0; dimension<dimensions; dimension++) {
                        dv[dimension] += scalar_s * alpha * s_vec[dimension] - scalar_v * beta * v_vec[dimension];
                    }
                }
            }
            fishes[i].avoidTank(dv);
            fishes[i].normalise_and_move(dv);
            window.draw(drawFish(fishes[i]));
        }
        window.display();
    }
}