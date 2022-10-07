//
// Created by Zimri on 3/10/2022.
//

#include <vector>
#include <algorithm>
#include <iostream>
#include "sim.h"
#include "graphics.h"
#include "orbit.h"
#include "obj.h"
#include "geometry.h"

std::shared_ptr<sim::Sim> instance = nullptr;

std::shared_ptr<sim::Sim> sim::GetSimInstance() {
    if (instance == nullptr) {
        instance = std::make_shared<sim::Sim>(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    return instance;
}

void sim::Sim::Draw(SDL_Renderer *surface) {
    SDL_SetRenderDrawColor(surface, 0xFF, 0xFF, 0xFF, 0xFF);
    for (auto &o: objects) {
        sim::Vec objScreenPos = GetScreenCoords(*o->pos);
        SDL_SetRenderDrawColor(surface, 0xFF, 0xFF, 0xFF, 0xFF);
        sim::graphics::DrawEllipse(surface, int(objScreenPos.x), int(objScreenPos.y),
                                   int(o->r * scale->x), int(o->r * scale->y));
        SDL_Rect highlight{int(objScreenPos.x) - 10, int(objScreenPos.y) - 10, 20, 20};
        if (o->orbit != nullptr && o->orbit->valid) {
            SDL_SetRenderDrawColor(surface, 0x33, 0xbb, 0x33, 0xff);
            sim::Vec aroundScreenPos =
                    GetScreenCoords(*o->orbit->around->pos) - o->orbit->eVec->Norm() * *scale * o->orbit->c;
            sim::graphics::DrawEllipse(surface, int(aroundScreenPos.x), int(aroundScreenPos.y),
                                       int(o->orbit->a * scale->x),
                                       int(o->orbit->b * scale->y), o->orbit->argOfPeriapsis);
        }
        if (o->selected) {
            SDL_SetRenderDrawColor(surface, 0xff, 0xff, 0xff, 0xff);
            // left side
            SDL_RenderDrawLine(surface, highlight.x - 4, highlight.y - 4, highlight.x - 4,
                               highlight.y + highlight.h + 4);
            // right side
            SDL_RenderDrawLine(surface, highlight.x + highlight.w + 4, highlight.y - 4, highlight.x + highlight.w + 4,
                               highlight.y + highlight.h + 4);
            // bottom right
            SDL_RenderDrawLine(surface, highlight.x + highlight.w + 4, highlight.y + highlight.h + 4,
                               highlight.x + highlight.w - 2, highlight.y + highlight.h + 4);
            // bottom left
            SDL_RenderDrawLine(surface, highlight.x - 4, highlight.y + highlight.h + 4, highlight.x + 2,
                               highlight.y + highlight.h + 4);
            // top left
            SDL_RenderDrawLine(surface, highlight.x - 4, highlight.y - 4, highlight.x + 2, highlight.y - 4);
            // top right
            SDL_RenderDrawLine(surface, highlight.x + highlight.w + 4, highlight.y - 4,
                               highlight.x + highlight.w - 2, highlight.y - 4);

        }
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
                            (4. / 3.) * PI * std::pow(o->r, 3.0) + (4. / 3.) * PI * std::pow(o2->r, 3.0);
                    auto merged = std::make_shared<sim::Obj>(new sim::Vec(*o->pos + offset),
                                                             new sim::Vec(momentum / totalMass),
                                                             totalMass, std::cbrt(3 * newVolume / (4 * PI))
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
                double f = G * (o->m * o2->m) / (dist * dist); // using gigameters and days for dist/time
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
    auto sun = std::make_shared<sim::Obj>(new sim::Vec{0., 0., 0.}, new sim::Vec{0., 0., 0.}, 1.9891e30, 0.696347055,
                                          "sun");
    objects.push_back(sun);
    for(auto& i : saved) {
        i = sun;
    }
    selected = sun;
    selectedIdx = 0;
    following = sun;
    sun->selected = true;
    objects.push_back(sim::Obj::MakeCircularized(*sun, 57.90905, 3.3011e23, 0.00488, "mercury"));
    objects.push_back(sim::Obj::MakeCircularized(*sun, 108.93900, 4.8675e24, 0.006051, "venus"));
    // jupiter
    objects.push_back(
            sim::Obj::MakeCircularized(*sun, 745.2, 1.89813e26, 0.0699115127, "jupiter"));
    auto earth = sim::Obj::MakeCircularized(*sun, 148.5971689, 5.9722e24, 0.00637107103, "earth");
    objects.push_back(earth);
    // moon
    objects.push_back(
            sim::Obj::MakeCircularized(*earth, 0.385, 7.34767309e22, 0.00173744778, "luna"));
    // mars
    objects.push_back(
            sim::Obj::MakeCircularized(*sun, 217.99, 6.39e23, 0.0033894394, "mars"));
}

sim::Sim::~Sim() {
    delete scale;
}

sim::Vec sim::Sim::GetWorldCoords(sim::Vec &screenCoords) {
    double x = (screenCoords.x - double(viewportWidth) / 2) / scale->x + following->pos->x;
    double y = (screenCoords.y - double(viewportHeight) / 2) / scale->y + following->pos->y;
    return {x, y, 0.};
}

sim::Vec sim::Sim::GetScreenCoords(sim::Vec &worldCoords) {
    double x = (worldCoords.x - following->pos->x) * scale->x + double(viewportWidth) / 2;
    double y = (worldCoords.y - following->pos->y) * scale->y + double(viewportHeight) / 2;
    return {x, y, 0.};
}

sim::Sim::Sim(int viewportWidth, int viewportHeight) : viewportHeight(viewportHeight), viewportWidth(viewportWidth) {
    for (auto &i: saved) {
        i = nullptr;
    }
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

sim::Vec sim::Sim::GetWorldCoords(sim::Vec &&screenCoords) {
    return GetWorldCoords(screenCoords);
}

sim::Vec sim::Sim::GetScreenCoords(sim::Vec &&worldCoords) {
    return GetScreenCoords(worldCoords);
}
