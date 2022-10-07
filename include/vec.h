//
// Created by Zimri on 3/10/2022.
//

#ifndef SDL_TUTORIAL_VEC_H
#define SDL_TUTORIAL_VEC_H

namespace sim {
    class Vec {
    public:
        const double x, y, z;

        Vec();

        Vec(double, double, double);

        Vec operator+(Vec other) const;

        Vec operator-(Vec other) const;

        Vec operator-() const;

        Vec operator/(Vec other) const;

        Vec operator/(double) const;

        Vec operator*(Vec &other) const;

        Vec operator*(double) const;

        Vec operator*(int) const;

        double operator[](int) const;

        Vec Abs() const;

        double Len() const;

        double Dist(Vec &other) const;

        Vec Norm() const;

        Vec Cross(Vec &other) const;

//        Vec Copy() const;
    };
};

#endif //SDL_TUTORIAL_VEC_H
