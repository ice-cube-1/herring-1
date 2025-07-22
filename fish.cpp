#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>

#define d_t 0.1
#define fishCount 50

#define sigma 0.1
#define alpha 0.05
#define beta 0.1
#define r 0.9  
#define p 2.5    
#define q 1.5       


#define dimensions 2

std::default_random_engine generator;
std::normal_distribution<double> distribution(0, sqrt(d_t));

class Fish {
    public:
    float x[dimensions];
    float v[dimensions];
    int uid;
    Fish () { 
        for (int i = 0; i<dimensions; i++) {
            x[i] = (rand()%400 + 200)/10.0f;
            v[i] = (rand() % 10 - 5) / 5.0f; 
        }
        uid = rand()%1000;
    }
    sf::Uint8 color_alpha() {
        return 255;
    }
};

float get_abs_difference(float a[dimensions], float b[dimensions]) {
    float ans = 0;
    for (int i = 0; i<dimensions; i++) {
        ans += std::pow(a[i]-b[i],2);
    }
    return std::pow(ans, 0.5);
}

void get_vector_difference(const float a[dimensions], const float b[dimensions], float result[dimensions]) {
    for (int i = 0; i < dimensions; i++) {
        result[i] = a[i] - b[i];
    }
}


int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "My window");
    Fish fishes[fishCount];
    for (int t = 0; t<100000; t++) {
        window.clear(sf::Color::White);


        for (int i = 0; i<fishCount; i ++) {
            float dv[2] = {0,0};
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

            for (int dimension = 0; dimension<dimensions; dimension++) {
                fishes[i].v[dimension] += dv[dimension]*d_t;
                float dx = distribution(generator)*sigma + fishes[i].v[dimension];
                fishes[i].x[dimension] += dx*d_t;
                std::cout << fishes[i].v[dimension] << dv[dimension] * d_t << "\n";
            }


            sf::CircleShape circle(1.f);
            circle.setFillColor(sf::Color{0, 150, 255, fishes[i].color_alpha()});
            circle.setPosition(fishes[i].x[0]*10, fishes[i].x[1]*10);
            window.draw(circle);
        }
        window.display();
    }
}