/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL.h>
#include <cstdio>
#include <string>
#include <cmath>
#include "graphics.h"
#include "sim.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 1000;

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window *gWindow = nullptr;

//The window renderer
SDL_Renderer *gRenderer = nullptr;

sim::Sim *instance = nullptr;

bool init() {
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    } else {
        //Create window
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == nullptr) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    instance = new sim::Sim(SCREEN_WIDTH, SCREEN_HEIGHT);
    instance->Init();
    return success;
}

void close() {
    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;
    delete instance;
    //Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char *args[]) {
    //Start up SDL and create window
    if (!init()) {
        printf("Failed to initialize!\n");
    } else {
        //Load media
        //Main loop flag
        bool quit = false;

        //Event handler
        SDL_Event e;
        double scaleFactor = 0.;
        double daysPerStepFactor = 1;
        sim::Vec *rightDragStart = nullptr;
        std::shared_ptr<sim::Obj> leftDragStart = nullptr;
        sim::Vec *mousePos = nullptr;

        //While application is running
        while (!quit) {
            //Handle events on queue
            while (SDL_PollEvent(&e) != 0) {
                //User requests quit
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_MOUSEWHEEL) {
                    double amt = (e.wheel.y > 0) ? 0.1 : -0.1;
                    scaleFactor += amt;
                    auto oldScale = instance->scale;
                    instance->scale = new sim::Vec(pow(2, scaleFactor), pow(2, scaleFactor), 0.);
                    delete oldScale;
                } else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP) {
                        daysPerStepFactor -= 0.1;
                    } else if (e.key.keysym.sym == SDLK_DOWN) {
                        daysPerStepFactor += 0.1;
                    }
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    sim::Vec coords{double(e.button.x), double(e.button.y), 0.};
                    if (e.button.button == 1) {
                        leftDragStart = instance->GetObjectAt(coords);
                    } else if (e.button.button == 3) {
                        rightDragStart = new sim::Vec(coords);
                    }
                } else if (e.type == SDL_MOUSEBUTTONUP) {
                    sim::Vec coords{double(e.button.x), double(e.button.y), 0.};
                    if (e.button.button == 3 && rightDragStart != nullptr) {
                        double mag = (rightDragStart->Dist(coords) / 512) /
                                     pow(2., scaleFactor);
                        sim::Vec vel = (coords - *rightDragStart) * mag + *instance->following->vel;
                        sim::Vec worldCoords = instance->GetWorldCoords(*rightDragStart);
                        instance->objects.push_back(
                                std::make_shared<sim::Obj>(new sim::Vec(worldCoords), new sim::Vec(vel),
                                                           instance->following->m / 10,
                                                           instance->following->r / 2));
                        delete rightDragStart;
                        rightDragStart = nullptr;
                    } else if (e.button.button == 1 && leftDragStart != nullptr) {
                        std::shared_ptr<sim::Obj> objAtMouse = instance->GetObjectAt(coords);
                        if (objAtMouse == leftDragStart &&
                            objAtMouse != nullptr) { // if click and release on same obj, follow it
                            instance->following = objAtMouse;
                        } else {
                            auto worldCoords = instance->GetWorldCoords(coords);
                            // throw it
                            // same distance on screen should be same speed on screen
                            double mag = (instance->GetScreenCoords(*leftDragStart->pos).Dist(coords) / 4) /
                                         pow(2., scaleFactor);
                            auto oldVel = leftDragStart->vel;
                            leftDragStart->vel = new sim::Vec(
                                    (worldCoords - *leftDragStart->pos).Norm() * mag + *oldVel);
                            delete oldVel;
                        }
                        leftDragStart = nullptr;
                    }
                } else if (e.type == SDL_MOUSEMOTION) {
                    delete mousePos;
                    mousePos = new sim::Vec{double(e.motion.x), double(e.motion.y), 0.};
                }
            }

            //Clear screen
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(gRenderer);
            instance->Draw(gRenderer);
            if (rightDragStart != nullptr && mousePos != nullptr) {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderDrawLine(gRenderer, rightDragStart->x, rightDragStart->y, mousePos->x, mousePos->y);
            }
            if (leftDragStart != nullptr && mousePos != nullptr) {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                auto screenCoords = instance->GetScreenCoords(*leftDragStart->pos);
                SDL_RenderDrawLine(gRenderer, screenCoords.x, screenCoords.y, mousePos->x, mousePos->y);
            }
            SDL_RenderPresent(gRenderer);
            instance->Step(pow(0.01, daysPerStepFactor));
        }
        delete rightDragStart;
        delete mousePos;
    }

    //Free resources and close SDL
    close();

    return 0;
}