//
// Created by Zimri on 3/10/2022.
//

#include <vector>
#include <algorithm>
#include "sim.h"
#include "graphics.h"
#include "obj.h"


void sim::Sim::Draw(SDL_Renderer *surface) {
    SDL_SetRenderDrawColor(surface, 0xFF, 0xFF, 0xFF, 0xFF);
    for (auto &o: objects) {
        sim::Vec objScreenPos = GetScreenCoords(*o->pos);
        DrawCircle(surface, int(objScreenPos.x), int(objScreenPos.y),
                   int(o->r * std::min(scale->x, scale->y)));//int(std::log(o->m) / std::log(3)) / 5);
        SDL_Rect highlight{int(objScreenPos.x) - 10, int(objScreenPos.y) - 10, 20, 20};
        SDL_SetRenderDrawColor(surface, 0x66, 0x66, 0x66, 0xFF);
        SDL_RenderDrawRect(surface, &highlight);
    }
}


void sim::Sim::Step(double days) {
    std::vector<std::shared_ptr<sim::Obj>> newObjs;
    for (auto &o: objects) {
        if (!o->exists) {
            continue;
        }
        for (auto &o2: objects) {
            if (!o2->exists) {
                continue;
            }
            if (&o != &o2) {
                if (o->IsCollidingWith(*o2)) {
                    // new object, conserve momentum, position is center of gravity of the two
                    // if the masses are equal, want to be at the midpoint between them
                    // weight position along the diff by mass
                    double totalMass = o->m + o2->m;
                    double massRatio = o->m / totalMass;
                    sim::Vec offset = (*o->pos - *o2->pos) * (1 - massRatio);
                    sim::Vec momentum = *o->vel * o->m + *o2->vel * o2->m;
                    double newVolume =
                            (4. / 3.) * 3.1415926 * std::pow(o->r, 3.0) + (4. / 3.) * 3.1415926 * std::pow(o2->r, 3.0);
                    auto merged = std::make_shared<sim::Obj>(new sim::Vec(*o->pos + offset),
                                                             new sim::Vec(momentum / totalMass),
                                                             totalMass, std::cbrt(3 * newVolume / (4 * 3.1415926))
                    );
                    o->exists = false;
                    o2->exists = false;
                    if (following == o || following == o2) {
                        following = merged;
                    }
                    newObjs.push_back(merged);
                    continue;
                }

                double dist = o->pos->Dist(*o2->pos);
                double f = 4.98217402e-28f * (o->m * o2->m) / (dist * dist); // using gigameters and days for dist/time
                // units kg*gigameter/day^2
                // want to convert to kg*gigameter/(days/day)^2
//                f /= (days * days);
                f *= days;
                sim::Vec fVec = (*o2->pos - *o->pos).Norm() * f;

                o->ApplyForce(fVec);
            }
        }
    }
    objects.erase(std::remove_if(objects.begin(), objects.end(), [](auto &o) { return !o->exists; }), objects.end());
    // is it really not ok to leave this return value?? it's a vec so i dont have to deal with it
    for (auto &o: objects) {
        o->Step(days);
    }
    if (!newObjs.empty()) {
        objects.insert(objects.end(), newObjs.begin(), newObjs.end());
        newObjs.clear();
    }
}

void sim::Sim::Init() {
    // sun
    auto sun = std::make_shared<sim::Obj>(new sim::Vec{0., 0., 0.}, new sim::Vec{0., 0., 0.}, 1.9891e30, 0.696347055);
    objects.push_back(sun);
    following = sun;
    objects.push_back(sim::Obj::MakeCircularized(*sun, 57.90905, 3.3011e23, 0.00488));
    objects.push_back(sim::Obj::MakeCircularized(*sun, 108.93900, 4.8675e24, 0.006051));
    // jupiter
    objects.push_back(
            sim::Obj::MakeCircularized(*sun, 745.2, 1.89813e26, 0.0699115127));
    auto earth = sim::Obj::MakeCircularized(*sun, 148.5971689, 5.9722e24, 0.00637107103);
    objects.push_back(earth);
    // moon
    objects.push_back(
            sim::Obj::MakeCircularized(*earth, 0.385, 7.34767309e22, 0.00173744778));
    // mars
    objects.push_back(
            sim::Obj::MakeCircularized(*sun, 217.99, 6.39e23, 0.0033894394));
}

sim::Sim::~Sim() {
    delete scale;
}

sim::Vec sim::Sim::GetWorldCoords(sim::Vec &screenCoords) {
    double x = (screenCoords.x - double(viewportHeight) / 2) / scale->x + following->pos->x;
    double y = (screenCoords.y - double(viewportHeight) / 2) / scale->y + following->pos->y;
    return {x, y, 0.};
}

sim::Vec sim::Sim::GetScreenCoords(sim::Vec &worldCoords) {
    double x = (worldCoords.x - following->pos->x) * scale->x + double(viewportHeight) / 2;
    double y = (worldCoords.y - following->pos->y) * scale->y + double(viewportHeight) / 2;
    return {x, y, 0.};
}

sim::Sim::Sim(int viewportWidth, int viewportHeight) : viewportHeight(viewportHeight), viewportWidth(viewportWidth) {
}

std::shared_ptr<sim::Obj>
sim::Sim::GetObjectAt(sim::Vec &screenCoords) {

    double bestDist = 20;
    std::shared_ptr<Obj> bestObj = nullptr;
    for (auto &obj: objects) {
        sim::Vec objScreenCoords = GetScreenCoords(*obj->pos);
        double dist = std::sqrt(
                pow(objScreenCoords.x - screenCoords.x, 2.0) + pow(objScreenCoords.y - screenCoords.y, 2.0));
        if (dist < bestDist) {
            bestObj = obj;
            bestDist = dist;
        }
    }
    return bestObj;
}
