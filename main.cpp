#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <random>

#define d_t 0.1
#define fishCount 200

#define sigma 0
#define alpha 0.05
#define beta 0.1
#define gamma 0.5
#define r 1.5  
#define p 2.5    
#define q 1.5       
#define max_dv 1.5 * d_t
#define max_v 0.6
#define min_v 0.03
#define tank_size 15
#define cell_count 10
#define cell_width tank_size / cell_count
#define dimensions 3
#define cos_fov -0.86602540378 // = cos 150 deg
#define r_p std::pow(r,p)
#define r_q std::pow(r,q)

std::default_random_engine generator;
std::normal_distribution<double> distribution(0, sqrt(d_t));

class Vec3 {
public:
    float arr[3];
    Vec3() : arr{0, 0, 0} {}
    Vec3(float xVal, float yVal, float zVal) : arr{xVal, yVal, zVal} {}
    float x() const { return arr[0]; }
    float y() const { return arr[1]; }
    float z() const { return arr[2]; }
    Vec3 operator+(const Vec3& other) const { return Vec3(x() + other.x(), y() + other.y(), z() + other.z()); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x() - other.x(), y() - other.y(), z() - other.z()); }
    Vec3 operator*(const float scalar) const { return Vec3(x()*scalar, y()*scalar, z()*scalar); }
    float abs() { return std::pow(x()*x()+y()*y()+z()*z(), 0.5); }
    float dot_product(const Vec3& other) const { return x() * other.x() + y() * other.y() + z() * other.z(); }
};

class Fish {
    public:
    Vec3 s;
    Vec3 v;
    Vec3 a;
    Fish () { 
        for (int i = 0; i<dimensions; i++) {
            s.arr[i] = (rand()%800)/80.0f;
            v.arr[i] = (rand() % 10 - 5) / 5.0f; 
            a.arr[i] = 0;
        }
    }
    sf::Uint8 color() {
        return 240-s.z()*10;
    }
    void avoidTank() {
        for (int dimension = 0; dimension<dimensions; dimension ++) {
            Vec3 reflection_vector;
            for (int d = 0; d < dimensions; d++) {
                if (d == dimension) { reflection_vector.arr[d] = -v.arr[d]; }
                else { reflection_vector.arr[d] = v.arr[d]; }
            }
            Vec3 reflection_pos;
            if (v.arr[dimension] < 0) {
                reflection_pos.arr[dimension] = 0;
            } else {
                reflection_pos.arr[dimension] = tank_size;
            }
            for (int other_d = 0; other_d < dimensions; other_d ++) {
                if (other_d != dimension) {
                    reflection_pos.arr[other_d] = (s.arr[dimension] * v.arr[other_d])/(reflection_pos.arr[dimension] - v.arr[dimension]) + s.arr[other_d];
                }
            }
            float abs_distance = (reflection_pos - s).abs();
            float scalar_collision = r_p/std::pow(abs_distance,p) + r_q/std::pow(abs_distance,q);
            a = a - (v - reflection_vector) * (gamma * scalar_collision);
        }
    }
    void school(Fish* other_fish) {
        float abs_distance = (other_fish->s - s).abs();
        float p_term = r_p/std::pow(abs_distance,p);
        float q_term = r_q/std::pow(abs_distance,q);
        float scalar_s = p_term - q_term;
        float scalar_v = p_term + q_term;
        Vec3 v_vec = v - other_fish->v;
        Vec3 s_vec = s - other_fish->s;
        a = a + s_vec * (scalar_s * alpha) - v_vec * (scalar_v * beta);
    }

    void normalise_and_move() {
        float abs_a = a.abs();
        if (abs_a > max_dv) {
            a = a * (max_dv / abs_a);
        }
        v = v+(a*d_t);
        float abs_v = v.abs();
        float scale_factor = 1;
        if (abs_v > max_v) {
            scale_factor = max_v / abs_v;
        } else if (abs_v < min_v) {
            scale_factor = min_v / abs_v;
        }
        v = v * scale_factor;
        for (int dimension = 0; dimension<dimensions; dimension++) {
            float dx = distribution(generator)*sigma + v.arr[dimension];
            s.arr[dimension] += dx*d_t;
        }
    }
    void assign_cell(std::vector<Fish*> cells[cell_count][cell_count][cell_count]) {
        cells[static_cast<int>(s.x() / cell_width)][static_cast<int>(s.y() / cell_width)][static_cast<int>(s.z() / cell_width)].push_back(this);
    }
};

void remove_unordered(std::vector<Fish*>& vec, Fish* value) {
    std::vector<Fish *>::iterator it = std::find(vec.begin(), vec.end(), value);
    *it = std::move(vec.back());
    vec.pop_back();
}

bool check_fish_visible(Fish* fish, Fish* to_check) {
    Vec3 difference = to_check->s - fish->s;
    float cos_theta = fish->v.dot_product(difference) / (difference.abs() * fish->v.abs());
    return cos_theta > cos_fov;
}

sf::CircleShape drawFish(Fish* fish) {
    sf::CircleShape circle(4.f);
    circle.setFillColor(sf::Color{0, fish->color(), 255});
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
                        fish->a = {0,0,0};
                        for (Fish* other_fish: fish_nearby) {
                            if (check_fish_visible(fish, other_fish)) {
                                fish->school(other_fish);
                            }
                        }
                        fish->avoidTank();
                        fish->normalise_and_move();
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