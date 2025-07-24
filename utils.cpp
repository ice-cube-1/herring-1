#include "utils.h"
#include <algorithm>

void remove_unordered(std::vector<Herring*>& vec, Herring* value) {
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        *it = std::move(vec.back());
        vec.pop_back();
    }
}
