#pragma once

#include <cmath>
#include <random>

constexpr double d_t = 0.1;

constexpr int herringCount = 300;
constexpr double sigma = 0;
constexpr double alpha = 0.05;
constexpr double beta = 0.1;
constexpr double gamma = 1;

constexpr double r = 0.9;
constexpr double p = 2.5;
constexpr double q = 1.5;

constexpr double max_dv = 1.5;
constexpr double max_v = 0.6;
constexpr double min_v = 0.03;

constexpr double tank_size = 15.0;
constexpr int cell_count = 10;
constexpr double cell_width = tank_size / cell_count;

constexpr int dimensions = 3;
constexpr double cos_fov = -0.86602540378;  // cos(150 degrees)

const double r_p = std::pow(r, p);
const double r_q = std::pow(r, q);

extern std::default_random_engine generator;
extern std::normal_distribution<double> distribution;

// PREY AVOIDANCE
const double r_1 = 0.5;
const double theta_1 = 2;
const double delta = 1;

// PREDATOR
const int predator_count = 5;
const double r_2 = 3;
const double theta_2 = 2;
const double gamma_1 = 0.2;
const double gamma_2 = 0.1;
const double max_v_cod = 0.2;
const double min_v_cod = 0.04;
const double max_a_cod = 0.4;