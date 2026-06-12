#ifndef ENGINE_H
#define ENGINE_H

#include "camera_work.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>
#include <stdint.h>

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1600

#define CLICK_RADIUS 20.0f

#define MAX_DEFECTS 500
#define DEFECT_SIZE 32
#define GRID_CELL_SIZE 2.0f
#define INV_GRID_CELL_SIZE 0.5f
#define DEFECT_FORCE 0.9f
#define DEFECT_ALPHA 0.005f
#define MAX_DEFECT_FORCE 8.0f

// TODO: rethink about MAX_PARTICLE (currently leave it here to test more)
#define MAX_PARTICLES 750
#define PARTICLE_SIZE 20
#define LONDON_LEN 42
#define COHERENT_LEN 14
#define CUTOF_P_TO_P_INTERACTION_LEN 500

#define MAX_SUPERCONDUCTING_TEMPERATURE 80 // K
#define MAX_CURRENT_DENSITY 7 // (x10^3) A/m2

typedef struct {
    float *pX;
    float *pY;
    uint16_t count;
} particleSystem;

typedef struct {
    float *pX;
    float *pY;
    int count;
} defectSystem;

typedef struct {
    float *forceX;
    float *forceY;
    uint16_t cols;
    uint16_t rows;
} forceGrid;

typedef struct {
    float temperature;
    float Jx;
    float Jy;
} simConfig;

// particle system
particleSystem *initParticleSys(void);
void destroyParticleSys(particleSystem *pSystem);

void createParticle(particleSystem *pSystem, float x, float y);
void annihilateParticle(particleSystem *pSystem, float x, float y);

// defect system
defectSystem* initDefectSys(void);
forceGrid* initForceGrid(const defectSystem *pDefectSys);
void updateForceGrid(forceGrid *grid, const defectSystem *pDefectSys);

void destroyDefectSys(defectSystem *pDefectSys);
void destroyForceGrid(forceGrid *grid);

void createDefect(forceGrid *pForceGrid, defectSystem *pDefectSys, float x, float y);
void annihilateDefect(forceGrid *pForceGrid, defectSystem *pDefectSys, float x, float y);

// simulate the physics
void updatePhysics(particleSystem *particleSys, forceGrid *defectForceGrid, const simConfig *config);

// render stuff
bool initSDLGraphics(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **particleTexture, SDL_Texture **defectTexture);
bool renderParticles(SDL_Renderer *renderer, SDL_Texture *texture, particleSystem *system, camera *cam);
bool renderDefects(SDL_Renderer *renderer, SDL_Texture *texture, defectSystem *system, camera *cam);

void destroyAllSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *particleTexture, SDL_Texture *defectTexture);

#endif