#include "consts.h"
#include <cmath>

std::default_random_engine generator;
std::normal_distribution<double> distribution(0, std::sqrt(d_t));

double alpha = 1.0;
double beta = 1.0;
double gamma_0 = 1.0;
double delta = 1.0;

void set_prey_params(std::array<double,4> x) {
    alpha = x[0];
    beta = x[1];
    gamma_0 = x[2];
    delta = x[3];
}

void set_predator_params(std::array<double,3> x) {
    gamma_1 = x[0];
    gamma_2 = x[1];
    k = x[2];
}