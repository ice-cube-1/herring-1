#include "herring.h"
#include <array>
#include "school.h"
School::School() {};
Vec3 School::average_s() const {
    Vec3 s;
    for (Herring* h: herring) { s = s + h->s; }
    return s * (1.0f / herring.size());
}
Vec3 School::average_v() const {
    Vec3 v;
    for (Herring* h: herring) { v = v + h->v; }
    return v * (1.0f / herring.size());
}
