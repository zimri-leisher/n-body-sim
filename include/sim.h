//
// Created by Zimri on 3/10/2022.
//

#ifndef SDL_TUTORIAL_SIM_H
#define SDL_TUTORIAL_SIM_H

#include <SDL.h>
#include <vector>
#include <memory>

#include "vec.h"
#include "obj.h"

namespace sim {
    class Sim {
    public:
        std::vector<std::shared_ptr<Obj>> objects;
        std::shared_ptr<Obj> following = nullptr;
        sim::Vec *scale = new sim::Vec{1., 1., 0.};
        int viewportWidth, viewportHeight;

        Sim(int viewportWidth, int viewportHeight);

        ~Sim();

        void Draw(SDL_Renderer *surface);

        void Step(double);

        sim::Vec GetWorldCoords(sim::Vec &screenCoords);

        sim::Vec GetScreenCoords(sim::Vec &worldCoords);

        std::shared_ptr<Obj> GetObjectAt(sim::Vec &pos);

        void Init();
    };
}
#endif //SDL_TUTORIAL_SIM_H
