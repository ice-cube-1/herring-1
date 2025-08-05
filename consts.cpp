#include "consts.h"
#include <cmath>

std::default_random_engine generator;
std::normal_distribution<double> distribution(0, std::sqrt(d_t));

double alpha = 1.0;
double beta = 1.0;
double gamma_0 = 1.0;
double delta = 1.0;

void set_params(std::array<double,4> x) {
    alpha = x[0];
    beta = x[1];
    gamma_0 = x[2];
    delta = x[3];
}