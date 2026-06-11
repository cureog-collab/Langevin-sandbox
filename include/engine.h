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
#define MAX_PARTICLES 5000
#define PARTICLE_SIZE 8

typedef struct {
    uint32_t capacity;
    uint32_t count;
    float *pX;
    float *pY;
} particleSystem;

// memory for particle system
particleSystem *initParticleSys(uint32_t inputCap);
void destroyParticleSys(particleSystem *pSystem);

// simulate the physics
bool updatePhysics(particleSystem *system, float temperature);

// render stuff
bool initSDLGraphics(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture, int textureW, int textureH);
bool renderParticles(SDL_Renderer *renderer, SDL_Texture *texture, particleSystem *system, camera *cam);

void destroyAllSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);

#endif