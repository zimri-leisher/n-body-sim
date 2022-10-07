//
// Created by Zimri on 3/10/2022.
//

#include <cmath>
#include "vec.h"
#include <stdexcept>

using namespace sim;

Vec::Vec() : x(0), y(0), z(0) {
}

Vec::Vec(double x, double y, double z) : x(x), y(y), z(z) {
}

Vec Vec::operator+(const Vec &other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vec Vec::operator-(const Vec &other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Vec Vec::operator*(double s) const {
    return {x * s, y * s, z * s};
}

Vec Vec::operator*(int s) const {
    return {x * double(s), y * double(s), z * double(s)};
}

Vec Vec::operator/(const Vec &other) const {
    return {x / other.x, y / other.y, z / other.z};
}

Vec Vec::operator*(const Vec &other) const {
    return {x * other.x, y * other.y, z * other.z};
}

double Vec::Dist(const Vec &other) const {
    return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
}

double Vec::Len() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vec Vec::Abs() const {
    return {std::abs(x), std::abs(y), std::abs(z)};
}

Vec Vec::Norm() const {
    double l = Len();
    if (l == 0.) {
        return {0., 0., 0.};
    }
    return {x / l, y / l, z / l};
}

Vec Vec::operator/(double s) const {
    return {x / s, y / s, z / s};
}

Vec Vec::operator-() const {
    return {-x, -y, -z};
}

Vec Vec::Cross(const Vec &other) const {
    return {y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x};
}

double Vec::operator[](int i) const {
    if (i < 0 || i > 2) {
        throw std::invalid_argument("i must be within [0, 2]");
    }
    if (i == 0) {
        return x;
    }
    if (i == 1) {
        return y;
    }
    return z;
}

double Vec::GetX() const {
    return x;
}

double Vec::GetY() const {
    return y;
}

double Vec::GetZ() const {
    return z;
}
