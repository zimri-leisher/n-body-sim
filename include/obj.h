//
// Created by Zimri on 3/10/2022.
//

#ifndef SDL_TUTORIAL_OBJ_H
#define SDL_TUTORIAL_OBJ_H

#include "vec.h"
#include <memory>
namespace sim {
    class Obj {
    public:
        sim::Vec* pos;
        sim::Vec* vel;
        double m;
        double r;
        bool exists = true;

        Obj(sim::Vec* pos, sim::Vec* vel, double m=1e10, double r=0.1);
        ~Obj();
        void ApplyForce(sim::Vec&);

        void Step(double);

        bool IsCollidingWith(const Obj& other) const;

        static std::shared_ptr<Obj> MakeCircularized(const Obj& around, double orbitalRadius, double mass, double radius);
    };
}

#endif //SDL_TUTORIAL_OBJ_H
