//
// Created by Zimri on 3/10/2022.
//

#include "graphics.h"
#include "geometry.h"
#include "sim.h"
#include <SDL.h>
#include <string>
#include <iostream>

void
sim::graphics::DrawEllipse(SDL_Renderer *renderer, int32_t centerX, int32_t centerY, int32_t radiusX, int32_t radiusY,
                           double rot) {
    int viewportWidth = sim::GetSimInstance()->viewportWidth;
    int viewportHeight = sim::GetSimInstance()->viewportHeight;
    SDL_Rect *screenRect = new SDL_Rect{0, 0, viewportWidth, viewportHeight};
    const int maxPrecision = 100;
    // the best we can see is 1 step per pixel in ellipse, so this is the best precision that would be visible
    // so calculate the perimeter (approx)
    int perimeter = (int) (2 * PI * sqrt((radiusX * radiusX + radiusY * radiusY) / 2.));
    // perimeter will overflow if radiusX and radiusY are big. just use maxPrecision in this case
    int precision;
    if (perimeter < 0) {
        precision = maxPrecision;
    } else {
        precision = std::min(perimeter, maxPrecision);
    }
    double step = 2 * PI / precision;
    int stepCt = 0;
    double t = step;
    int initX = (int) (radiusX * cos(rot)), initY = (int) (radiusX * sin(rot));
    int x1 = initX;
    int y1 = initY;
    int x2, y2;
    int linesDrawn = 0;
    for (; stepCt <= precision; stepCt++) {
        double pX = radiusX * cos(t);
        double pY = radiusY * sin(t);
        x2 = (int) (pX * cos(rot) - pY * sin(rot));
        y2 = (int) (pX * sin(rot) + pY * cos(rot));
        int lx1 = x1 + centerX, ly1 = y1 + centerY, lx2 = x2 + centerX, ly2 = y2 + centerY;
        if (SDL_IntersectRectAndLine(screenRect, &lx1, &ly1, &lx2, &ly2) == SDL_TRUE) {
            // don't draw line if it's not inside screen
            SDL_RenderDrawLine(renderer, x1 + centerX, y1 + centerY, x2 + centerX, y2 + centerY);
        }
        x1 = x2;
        y1 = y2;
        t = step * stepCt;
    }
    int lx1 = x1 + centerX, ly1 = y1 + centerY, lx2 = initX + centerX, ly2 = initY + centerY;
    if (SDL_IntersectRectAndLine(screenRect, &lx1, &ly1, &lx2, &ly2) == SDL_TRUE) {
        SDL_RenderDrawLine(renderer, x1 + centerX, y1 + centerY, initX + centerX, initY + centerY);
    }
}