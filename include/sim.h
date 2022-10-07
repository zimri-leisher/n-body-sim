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

const double G = 4.98217402e-28f; // gigameters^3 kg^-1 days^-2 LOL
const double PI = 3.14159265358979;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;


namespace sim {

    class Sim {
    public:
        std::vector<std::shared_ptr<Obj>> objects;
        std::shared_ptr<Obj> following = nullptr;
        std::shared_ptr<Obj> selected = nullptr;
        int selectedIdx = -1;
        sim::Vec *scale = new sim::Vec{1., 1., 0.};
        int viewportWidth, viewportHeight;
        std::vector<std::shared_ptr<Obj>> saved{10};

        Sim(int viewportWidth, int viewportHeight);

        ~Sim();

        void Draw(SDL_Renderer *surface);

        void Step(double);

        sim::Vec GetWorldCoords(sim::Vec &screenCoords);

        sim::Vec GetWorldCoords(sim::Vec &&screenCoords);

        sim::Vec GetScreenCoords(sim::Vec &worldCoords);

        sim::Vec GetScreenCoords(sim::Vec &&worldCoords);

        std::shared_ptr<Obj> GetObjectAt(sim::Vec &pos);

        void Init();
    };

    std::shared_ptr<sim::Sim> GetSimInstance();
}
#endif //SDL_TUTORIAL_SIM_H
