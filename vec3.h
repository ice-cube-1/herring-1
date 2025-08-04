#include <string>
#ifndef VEC3_H
#define VEC3_H

class Vec3 {
public:
    float arr[3];
    Vec3();
    Vec3(float, float, float);
    float x() const;
    float y() const;
    float z() const;
    std::string to_string() const;
    Vec3 operator+(const Vec3&) const;
    Vec3 operator-(const Vec3&) const;
    Vec3 operator*(float) const;
    bool operator==(const Vec3&) const;
    bool operator!=(const Vec3&) const;
    float abs();
    float dot_product(const Vec3&) const;
};

#endif
