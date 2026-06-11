#include "../include/engine.h"
#include "../include/camera_work.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // check input command line arguments ================================================
    uint32_t initialCount = 20;
    if (argc == 2) 
    {
        int inputInitialCount = atoi(argv[1]);
        if (inputInitialCount > MAX_PARTICLES)
        {
            printf("Error: numbers of particles cannot be greater than %i!\n", MAX_PARTICLES);
            return 1;
        }
        else if (inputInitialCount < 0)
        {
            printf("Error: numbers of particles cannot be negative!\n");
            return 1;
        }
        initialCount = inputInitialCount;
    }
    else if (argc > 2)
    {
        printf("Usage: %s [initialCount]\n", argv[0]);
        return 1;
    }

    // intialize the engine ===============================================================
    // initialize SDL-related stuff
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error: failed to SDL_Init!\n");
        return 1;
    }
    SDL_Window *mainWindow = NULL;
    SDL_Renderer *mainRenderer = NULL;
    SDL_Texture *particleTexture = NULL;
    if (!initSDLGraphics(&mainWindow, &mainRenderer, &particleTexture, PARTICLE_SIZE, PARTICLE_SIZE))
    {
        printf("Error: failed to initiate mainWindow and mainRenderer!\n");
        return 1;
    }

    srand(time(NULL));

    // initialize system of particles =====================================================
    particleSystem *mainParticleSys = initParticleSys(MAX_PARTICLES);
    if (mainParticleSys == NULL)
    {
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainWindow);
        printf("Error: failed to generate mainParticleSys!\n");
        SDL_Quit();
        return 1;
    }
    for (uint32_t i = 0; i < initialCount; i++)
    {
        mainParticleSys->pX[i] = ((float)rand() / RAND_MAX) * WINDOW_WIDTH;
        mainParticleSys->pY[i] = ((float)rand() / RAND_MAX) * WINDOW_HEIGHT;
        mainParticleSys->count++;
    }

    // main program loop ==================================================================
    camera mainCam;
    SDL_Point origin = {0, 0};
    resetCamera(&mainCam, origin);
    bool isOn = true;
    bool isPausing = false;
    SDL_Event mainEvent;
    while (isOn)
    {
        // poll mainEvent loop
        while (SDL_PollEvent(&mainEvent))
        {
            switch (mainEvent.type)
            {
                // quit the program
                case SDL_QUIT:
                    isOn = false;
                    break;
                
                case SDL_KEYDOWN:
                {
                    // if the user click SPACE, reset camera settings
                    if (mainEvent.key.keysym.sym == SDLK_SPACE)
                    {
                        resetCamera(&mainCam, origin);
                    }

                    // pause the simulation
                    else if (mainEvent.key.keysym.sym == SDLK_s)
                    {
                        isPausing = !isPausing;
                    }
                    break;
                }
            }

            updateViewport(&mainCam, &mainEvent);
        }

        if (!isPausing)
        {
            // TODO
            // physics stuff
        }

        // TODO
        // graphic stuff
        SDL_SetRenderDrawColor(mainRenderer, 15, 15, 15, 255); 
        SDL_RenderClear(mainRenderer);

        renderParticles(mainRenderer, particleTexture, mainParticleSys, &mainCam);

        SDL_RenderPresent(mainRenderer);

        SDL_Delay(16);
    }

    destroyParticleSys(mainParticleSys);
    destroyAllSDL(mainWindow, mainRenderer, particleTexture);
    SDL_Quit();
    return 0;
}