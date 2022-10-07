
#ifndef N_BODY_SIM_ORBIT_H
#define N_BODY_SIM_ORBIT_H

#include <memory>
#include "obj.h"

namespace sim {
    class Orbit {
    public:
        bool valid = false;
        sim::Vec *eVec = nullptr, *r = nullptr;
        double e, a, b, c, T, argOfPeriapsis, mu;
        std::shared_ptr<sim::Obj> satellite;
        std::shared_ptr<sim::Obj> around;

        explicit Orbit(std::shared_ptr<sim::Obj> &s);

        ~Orbit();
    };
}

#endif //N_BODY_SIM_ORBIT_H
