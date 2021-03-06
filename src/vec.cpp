//
// Created by Zimri on 3/10/2022.
//

#include <cmath>
#include "vec.h"

sim::Vec::Vec() : x(0), y(0), z(0) {
}

sim::Vec::Vec(double x, double y, double z) : x(x), y(y), z(z) {
}

sim::Vec sim::Vec::operator+(sim::Vec other) const {
    return {x + other.x, y + other.y, z + other.z};
}

sim::Vec sim::Vec::operator-(sim::Vec other) const {
    return {x - other.x, y - other.y, z - other.z};
}

sim::Vec sim::Vec::operator*(double s) const {
    return {x * s, y * s, z * s};
}

sim::Vec sim::Vec::operator*(int s) const {
    return {x * double(s), y * double(s), z * double(s)};
}

sim::Vec sim::Vec::operator/(sim::Vec other) const {
    return {x / other.x, y / other.y, z / other.z};
}

sim::Vec sim::Vec::operator*(sim::Vec &other) const {
    return {x * other.x, y * other.y, z * other.z};
}

double sim::Vec::Dist(sim::Vec &other) const {
    return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z));
}

double sim::Vec::Len() const {
    return std::sqrt(x * x + y * y + z * z);
}

sim::Vec sim::Vec::Abs() const {
    return {std::abs(x), std::abs(y), std::abs(z)};
}

sim::Vec sim::Vec::Norm() const {
    double l = Len();
    if (l == 0.) {
        return {0., 0., 0.};
    }
    return {x / l, y / l, z / l};
}

sim::Vec sim::Vec::operator/(double s) const {
    return {x / s, y / s, z / s};
}

sim::Vec sim::Vec::operator-() const {
    return {-x, -y, -z};
}
