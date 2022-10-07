//
// Created by Zimri on 3/10/2022.
//

#ifndef SDL_TUTORIAL_GRAPHICS_H
#define SDL_TUTORIAL_GRAPHICS_H

#include <SDL.h>
#include <cstdio>
#include <string>
#include <cmath>
#include "graphics.h"

namespace sim {
    namespace graphics {
        void
        DrawEllipse(SDL_Renderer *renderer, int32_t centerX, int32_t centerY, int32_t radiusX, int32_t radiusY,
                    double rot = 0.);
    }
}

#endif //SDL_TUTORIAL_GRAPHICS_H
