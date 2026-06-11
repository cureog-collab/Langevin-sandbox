#ifndef ENGINE_H
#define ENGINE_H

#include "camera_work.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>
#include <stdint.h>

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1200

// TODO: rethink about MAX_PARTICLE (currently leave it here to test more)
#define MAX_PARTICLES 4000
#define PARTICLE_SIZE 8

#define MAX_SUPERCONDUCTING_TEMPERATURE 150 // K
#define MAX_CURRENT_DENSITY 50 // (x10^3) A/m2

typedef struct {
    float *pX;
    float *pY;
    uint16_t capacity;
    uint16_t count;
} particleSystem;

typedef struct {
    float temperature;
    float Jx;
    float Jy;
} simConfig;

// memory for particle system
particleSystem *initParticleSys(uint32_t inputCap);
void destroyParticleSys(particleSystem *pSystem);

// simulate the physics
void updatePhysics(particleSystem *system, const simConfig *config);

// render stuff
bool initSDLGraphics(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture);
bool renderParticles(SDL_Renderer *renderer, SDL_Texture *texture, particleSystem *system, camera *cam);

void destroyAllSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);

#endif