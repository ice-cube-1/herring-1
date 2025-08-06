#include "consts.h"
#include <cmath>

double alpha = 1.0;
double beta = 1.0;
double gamma_0 = 1.0;
double delta = 1.0;

double gamma_1 = 0.1;
double gamma_2 = 0.2;
double k = 0;

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