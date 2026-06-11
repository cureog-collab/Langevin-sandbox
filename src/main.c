#include "../include/engine.h"
#include "../include/camera_work.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VALID_FLAGS "c:t:x:y:"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // default values =====================================================================
    uint16_t initialCount = 20;
    simConfig mainConfig = {
        4.2f, // K
        0.0f, // (x10^3)A/m2
        1.0f // (x10^3)A/m2
    };
    // =====================================================================================

    // check input command line arguments ==================================================
    int opt;
    while ((opt = getopt(argc, argv, VALID_FLAGS)) != -1)
    {
        switch (opt)
        {
            case 't':
            {
                char *endptr = NULL;
                float inputTemperature = strtof(optarg, &endptr);
                if (endptr == optarg || *endptr != '\0') 
                {
                    printf("Error: mainConfig.temperature (K) must be a real number between 0 and %i!\n", MAX_SUPERCONDUCTING_TEMPERATURE);
                    return 1;
                }
                if (inputTemperature > MAX_SUPERCONDUCTING_TEMPERATURE)
                {
                    printf("Warning: mainConfig.temperature %.1f K exceeds the superconductivity threshold!\nAutomatically set to %i K.\n",
                            inputTemperature, MAX_SUPERCONDUCTING_TEMPERATURE);
                    mainConfig.temperature = MAX_SUPERCONDUCTING_TEMPERATURE;
                }
                else if (inputTemperature < 0.0f)
                {
                    printf("Warning: mainConfig.temperature cannot be negative!\nAutomatically set to 0 K.\n");
                    mainConfig.temperature = 0;
                }
                else
                {
                    mainConfig.temperature = inputTemperature;
                }
                break;
            }  
            
            case 'c':
            {
                char *endptr = NULL;
                int inputCount = strtol(optarg, &endptr, 10);
                if (endptr == optarg || *endptr != '\0') 
                {
                    printf("Error: numbers of particles must be an integer between 0 and %i!\n", MAX_PARTICLES);
                    return 1;
                }
                if (inputCount > MAX_PARTICLES)
                {
                    printf("Warning: numbers of particles cannot be greater than %i!\nAutomatically set to %i.\n", 
                            MAX_PARTICLES, MAX_PARTICLES);
                    initialCount = MAX_PARTICLES;
                }
                else if (inputCount < 0)
                {
                    printf("Warning: numbers of particles cannot be negative!\nAutomatically set to 20.\n");
                }
                else
                {
                    initialCount = (uint16_t)inputCount;
                }
                break;
            }

            case 'x':
            {
                char *endptr = NULL;
                float inputJx = strtof(optarg, &endptr);
                if (endptr == optarg || *endptr != '\0') 
                {
                    printf("Error: current density Jx (unit: x10^3 A/m^2) must be a float between 0 and %i!\n", MAX_CURRENT_DENSITY);
                    return 1;
                }
                if (fabsf(inputJx) > MAX_CURRENT_DENSITY)
                {
                    printf("Warning: current density Jx cannot be greater than %ix10^3 A/m^2!\nAutomatically set to %i.\n", 
                            MAX_CURRENT_DENSITY, MAX_CURRENT_DENSITY);
                    mainConfig.Jx = MAX_CURRENT_DENSITY;
                }
                else
                {
                    mainConfig.Jx = inputJx;
                }
                break;
            }

            case 'y':
            {
                char *endptr = NULL;
                float inputJy = strtof(optarg, &endptr);
                if (endptr == optarg || *endptr != '\0') 
                {
                    printf("Error: current density Jy (unit: x10^3 A/m^2) must be a float between 0 and %i!\n", MAX_CURRENT_DENSITY);
                    return 1;
                }
                if (fabsf(inputJy) > MAX_CURRENT_DENSITY)
                {
                    printf("Warning: current density Jy cannot be greater than %ix10^3 A/m^2!\nAutomatically set to %i.\n", 
                            MAX_CURRENT_DENSITY, MAX_CURRENT_DENSITY);
                    mainConfig.Jy = MAX_CURRENT_DENSITY;
                }
                else
                {
                    mainConfig.Jy = inputJy;
                }
                break;
            }

            case '?':
            {
                printf("Usage: %s [flags]\n", argv[0]);
                return 1;
            }
        }
    }
    // =====================================================================================

    // intialize the engine ================================================================
    // initialize SDL-related stuff
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error: failed to SDL_Init!\n");
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *mainWindow = NULL;
    SDL_Renderer *mainRenderer = NULL;
    SDL_Texture *particleTexture = NULL;
    if (!initSDLGraphics(&mainWindow, &mainRenderer, &particleTexture))
    {
        printf("Error: failed to initiate mainWindow and mainRenderer!\n");
        return 1;
    }
    // ====================================================================================

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
    // =====================================================================================

    // main program loop ===================================================================
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

                case SDL_MOUSEBUTTONDOWN:
                {
                    if (mainEvent.button.button == SDL_BUTTON_RIGHT)
                    {
                        int mouseX = mainEvent.button.x;
                        int mouseY = mainEvent.button.y;

                        float worldX = (mouseX - mainCam.camPos.x) / mainCam.zoom;
                        float worldY = (mouseY - mainCam.camPos.y) / mainCam.zoom;
                        if (SDL_GetModState() & KMOD_CTRL)
                        {
                            annihilateParticle(mainParticleSys, worldX, worldY);
                            break;
                        }
                        else
                        {
                            createParticle(mainParticleSys, worldX, worldY);
                            break;
                        }
                    }
                    break;
                }
            }

            updateViewport(&mainCam, &mainEvent);
        }

        if (!isPausing)
        {
            // physics stuff
            updatePhysics(mainParticleSys, &mainConfig);
        }

        // graphic stuff
        SDL_SetRenderDrawColor(mainRenderer, 145, 205, 235, 255); 
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