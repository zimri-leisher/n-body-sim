/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL.h>
#include <cstdio>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>
#include "graphics.h"
#include "sim.h"
#include "orbit.h"

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window *gWindow = nullptr;

//The window renderer
SDL_Renderer *gRenderer = nullptr;

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
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == nullptr) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    auto instance = sim::GetSimInstance();
    instance->Init();
    return success;
}

void close() {
    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;
    //Quit SDL subsystems
    SDL_Quit();
}

double scaleFactor = 0.;
double daysPerStepFactor = 1;
sim::Vec *rightDragStart = nullptr;
std::shared_ptr<sim::Obj> leftDragStart = nullptr;
sim::Vec *mousePos = nullptr;
sim::Vec *spaceDragStart = nullptr;

void processInputOnSelected(SDL_Event &e, std::shared_ptr<sim::Obj> &selected) {
    if(e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_RETURN) {
            sim::GetSimInstance()->following = selected;
        } else if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) {
            int slot = e.key.keysym.sym - SDLK_0;
            if (e.key.keysym.mod & KMOD_SHIFT) {
                sim::GetSimInstance()->saved[slot] = selected;
            } else {
                sim::GetSimInstance()->following = sim::GetSimInstance()->saved[slot];
            }
        } else if(e.key.keysym.sym == SDLK_SPACE) {
//            spaceDragStart = sim::GetSimInstance()->following->pos;

        }
    }
}

int main(int argc, char *args[]) {
    //Start up SDL and create window
    if (!init()) {
        printf("Failed to initialize!\n");
    } else {
        bool quit = false;

        //Event handler
        SDL_Event e;

        int fps = 0;
        uint64_t lastSec = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        auto instance = sim::GetSimInstance();
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
                        daysPerStepFactor += 0.6;
                    } else if (e.key.keysym.sym == SDLK_DOWN) {
                        daysPerStepFactor -= 0.6;
                    } else if (e.key.keysym.sym == SDLK_r) {
                        for (auto &obj: instance->objects) {
                            obj->orbit = std::make_shared<sim::Orbit>(obj);
                        }
                    } else if (e.key.keysym.sym == SDLK_TAB) {
                        instance->selectedIdx = (instance->selectedIdx + 1) % instance->objects.size();
                        if (instance->selected != nullptr) {
                            instance->selected->selected = false;
                        }
                        instance->selected = instance->objects[instance->selectedIdx];
                        instance->selected->selected = true;
                    } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                        instance->selected->selected = false;
                        instance->selected = nullptr;
                        instance->selectedIdx = -1;
                    } else if (instance->selected != nullptr) {
                        processInputOnSelected(e, instance->selected);
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
                        auto worldCoords = instance->GetWorldCoords(coords);
                        // throw it
                        // same distance on screen should be same speed on screen
                        double mag = (instance->GetScreenCoords(*leftDragStart->pos).Dist(coords) / 4) /
                                     pow(2., scaleFactor);
                        auto oldVel = leftDragStart->vel;
                        leftDragStart->vel = new sim::Vec(
                                (worldCoords - *leftDragStart->pos).Norm() * mag + *oldVel);
                        delete oldVel;
                        leftDragStart->orbit = std::make_shared<sim::Orbit>(leftDragStart);
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
            fps++;
            uint64_t curTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            if (curTime - lastSec >= 1000) {
                std::cout << "FPS: " << fps << std::endl;
                fps = 0;
                lastSec = curTime;
            }
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
            double daysPerFrame = pow(1.2, daysPerStepFactor) - 1;
            instance->Step(daysPerFrame / 2);
            instance->Step(daysPerFrame / 2);
            for (auto &obj: instance->objects) {
                obj->orbit = std::make_shared<sim::Orbit>(obj);
            }
        }
        delete rightDragStart;
        delete mousePos;
    }
    //Free resources and close SDL
    close();

    return 0;
}
