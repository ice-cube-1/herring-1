#include "utils.h"
#include <algorithm>

void remove_unordered(std::vector<Herring*>& vec, Herring* value) {
    std::vector<Herring *>::iterator it = std::find(vec.begin(), vec.end(), value);
    *it = std::move(vec.back());
    vec.pop_back();
}