//
// Created by Zimri on 3/10/2022.
//

#include <cstdio>
#include "obj.h"
#include "sim.h"
#include "orbit.h"
#include <cmath>
#include <memory>
#include <iostream>
#include <utility>

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

sim::Obj::Obj(sim::Vec *pos, sim::Vec *vel, double m, double r, std::string name) : pos(pos), vel(vel), m(m), r(r),
                                                                                    name(std::move(name)) {
}

std::shared_ptr<sim::Obj>
sim::Obj::MakeCircularized(const sim::Obj &around, double orbitalRadius, double mass, double radius,
                           std::string &&name) {
    // v^2 = (G * M_around) / R
    double vel = std::sqrt((G * around.m) / orbitalRadius);
    return std::make_shared<sim::Obj>(
            new sim::Vec{around.pos->GetX() + orbitalRadius, around.pos->GetY(), around.pos->GetZ()},
            new sim::Vec{around.vel->GetX(), around.vel->GetY() + vel, around.vel->GetZ()}, mass, radius, name);
}

bool sim::Obj::IsCollidingWith(const sim::Obj &other) const {
    return pos->Dist(*other.pos) < r + other.r;
}