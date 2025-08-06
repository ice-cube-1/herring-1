#pragma once

#include <cmath>
#include <random>
#include <array>

void set_prey_params(std::array<double,4> x);
void set_predator_params(std::array<double,3> x);

constexpr double d_t = 0.1;

constexpr int herringCount = 300;
constexpr double sigma = 0.2;
extern double alpha;
extern double beta;
extern double gamma_0;

constexpr double r = 0.5;
constexpr double p = 1.5;
constexpr double q = 2.5;

constexpr double max_dv = 1.5;
constexpr double max_v = 0.6;
constexpr double min_v = 0.03;

constexpr double tank_size = 15.0;
constexpr int cell_count = 10;
constexpr double cell_width = tank_size / cell_count;
constexpr double vision_range_herring = 5;

constexpr int dimensions = 3;
constexpr double cos_fov = -0.86602540378;  // cos(150 degrees)

const double r_p = std::pow(r, p);
const double r_q = std::pow(r, q);
const float epsilon = 1e-6f;

// PREY AVOIDANCE
const double r_1 = 3;
const double theta_1 = 1.5;
extern double delta;

// PREDATORS
const int predator_count = 3;
const float vision_range_cod = 15;
const float cod_body_length = 0.38;
const double r_2 = 3;
const double theta_2 = 2;
extern double gamma_1;
extern double gamma_2;
const double max_a_cod = 0.6;
extern double k;