//
// Created by Zimri on 3/10/2022.
//

#ifndef SDL_TUTORIAL_VEC_H
#define SDL_TUTORIAL_VEC_H

namespace sim {
    /**
     * A 3D immutable vector class. Operators work elementwise.
     */
    class Vec {
    private:
        const double x, y, z;
    public:
        Vec();

        Vec(double, double, double);

        double GetX() const;

        double GetY() const;

        double GetZ() const;

        Vec operator+(const Vec &other) const;

        Vec operator-(const Vec &other) const;

        Vec operator-() const;

        Vec operator/(const Vec &other) const;

        Vec operator/(double) const;

        Vec operator*(const Vec &other) const;

        Vec operator*(double) const;

        Vec operator*(int) const;

        double operator[](int) const;

        Vec Abs() const;

        double Len() const;

        double Dist(const Vec &other) const;

        Vec Norm() const;

        Vec Cross(const Vec &other) const;
    };
};

#endif //SDL_TUTORIAL_VEC_H
