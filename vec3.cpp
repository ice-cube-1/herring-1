#include <cmath>
#include "vec3.h"
#include <string>

Vec3::Vec3() : arr{0, 0, 0} {}

Vec3::Vec3(float xVal, float yVal, float zVal) : arr{xVal, yVal, zVal} {}

float Vec3::x() const { return arr[0]; }
float Vec3::y() const { return arr[1]; }
float Vec3::z() const { return arr[2]; }

Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3(x() + other.x(), y() + other.y(), z() + other.z());
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(x() - other.x(), y() - other.y(), z() - other.z());
}

Vec3 Vec3::operator*(float scalar) const {
    return Vec3(x() * scalar, y() * scalar, z() * scalar);
}

bool Vec3::operator==(const Vec3& other) const {
    return x() == other.x() && y() == other.y() && z() == other.z();
}

bool Vec3::operator!=(const Vec3& other) const {
    return !(*this == other);
}

float Vec3::abs() {
    return std::sqrt(x()*x() + y()*y() + z()*z());
}

float Vec3::dot_product(const Vec3& other) const {
    return x() * other.x() + y() * other.y() + z() * other.z();
}

std::string Vec3::to_string() const {
    return "X: " + std::to_string(x()) + ", Y: " + std::to_string(y()) + ", Z: " + std::to_string(z());
}
