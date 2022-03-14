//
// Created by Zimri on 3/10/2022.
//

#include <cstdio>
#include "obj.h"
#include <cmath>
#include <memory>

void sim::Obj::ApplyForce(sim::Vec &force) {
    auto *newVel = new sim::Vec(
            force / m + *vel); // ok, so it seems to me like immutable data types like this are really annoying
    // because now we have to allocate on the stack a ton??
    sim::Vec *oldVel = vel;
    vel = newVel;
    delete oldVel;
}

void sim::Obj::Step(double days) {
    sim::Vec adjVel = *vel * days;
    auto *newPos = new sim::Vec(*pos + adjVel);
    sim::Vec *oldPos = pos;
    pos = newPos;
    delete oldPos;
}

sim::Obj::~Obj() {
    delete pos;
    delete vel;
}

sim::Obj::Obj(sim::Vec *pos, sim::Vec *vel, double m, double r) : pos(pos), vel(vel), m(m), r(r) {
}

std::shared_ptr<sim::Obj>
sim::Obj::MakeCircularized(const sim::Obj &around, double orbitalRadius, double mass, double radius) {
    // v^2 = (G * M_around) / R
    double vel = std::sqrt((4.98233825e-28 * around.m) / orbitalRadius);
    return std::make_shared<sim::Obj>(new sim::Vec{around.pos->x + orbitalRadius, around.pos->y, around.pos->z},
                                      new sim::Vec{around.vel->x, around.vel->y + vel, around.vel->z}, mass, radius);
}

bool sim::Obj::IsCollidingWith(const sim::Obj &other) const {
    return pos->Dist(*other.pos) < r + other.r;
}


