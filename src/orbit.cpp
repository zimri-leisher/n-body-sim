
#include <utility>
#include <valarray>
#include <iostream>
#include "orbit.h"
#include "sim.h"

sim::Orbit::Orbit(std::shared_ptr<sim::Obj> &s) : satellite(s) {
    // go through all other bodies
    // we orbit around the one which at this velocity we will not escape from
    // and some more criteria but that's a start
    // we will orbit in some manner around all of those bodies
    // just have to select the "smallest" one in the hierarchy?

    // is current energy of this body more than escape energy?
    // what is escape velocity?
    // point at which kinetic and gravitational potential are equal
    std::vector<std::shared_ptr<sim::Obj>> gravitationallyBound;
    for (auto &body: sim::GetSimInstance()->objects) {
        if (body == satellite) {
            continue;
        }
        sim::Vec relVel = *satellite->vel - *body->vel;
        double Ke = satellite->m * pow(relVel.Len(), 2.0) / 2.0;
        double U = (G * body->m * satellite->m) / (body->pos->Dist(*satellite->pos));
        if (Ke < U) {
            gravitationallyBound.push_back(body);
        }
    }
    if (gravitationallyBound.empty()) {
        valid = false;
        around = nullptr;
        eVec = nullptr;
        a = T = e = b = argOfPeriapsis = c = 0.;
    } else {
        T = std::numeric_limits<double>::max();
        for (auto &body: gravitationallyBound) {
            // do some calculation here. if this object has a better period, use it
            sim::Vec r = *satellite->pos - *body->pos;
            double dist = r.Len();
            double mu = G * (body->m + satellite->m);
            sim::Vec relVel = *satellite->vel - *body->vel;
            double specificOrbitalEnergy = pow(relVel.Len(), 2.0) / 2 - mu / dist;
            double a = -mu / (2 * specificOrbitalEnergy);
            double T = sqrt((4 * PI * PI) / mu * a * a * a);
            if (T < this->T) {
                this->T = T;
                this->a = a;
                this->mu = mu;
                this->r = new sim::Vec(r);
                around = body;
            }
        }
        // ok figure it out. we want the semi-major axis. if we can find the furthest away this object will be... which
        // would be when GPE is highest and KE is lowest
        sim::Vec relVel = *satellite->vel - *around->vel;
        sim::Vec hVec = r->Cross(relVel);
        eVec = new sim::Vec((relVel.Cross(hVec) / mu) - r->Norm());
        e = eVec->Len();
        b = a * sqrt(1 - e * e);
        c = sqrt(a * a - b * b);
        argOfPeriapsis = atan2(eVec->y, eVec->x);
        valid = true;
    }
}

sim::Orbit::~Orbit() {
    delete eVec, r;
}
