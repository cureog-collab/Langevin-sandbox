#include "../include/engine.h"
#include <math.h>
#include <stdlib.h>

#define TEMPERATURE_SCALE 0.8f
#define LORENTZ_SCALE 5.0f

const float INV_WINDOW_WIDTH = 1.0f / WINDOW_WIDTH;
const float INV_WINDOW_HEIGHT = 1.0f / WINDOW_HEIGHT;

static inline void getThermalNoise(float *jumpX, float *jumpY, float temperature);

void updatePhysics(particleSystem *system, const simConfig *config)
{
    for (int particleIdx = 0; particleIdx < system->count; particleIdx++)
    {
        float temperature = config->temperature;
        float jumpX = 0.0f;
        float jumpY = 0.0f;

        // temperature
        getThermalNoise(&jumpX, &jumpY, temperature);

        // Lorentz force
        jumpX += config->Jy * LORENTZ_SCALE;
        jumpY -= config->Jx * LORENTZ_SCALE;

        // asign jump to the current position of the pariticle
        system->pX[particleIdx] += jumpX;
        system->pY[particleIdx] += jumpY;

        // wrap-around at edges
        system->pX[particleIdx] -= WINDOW_WIDTH * floorf(system->pX[particleIdx] * INV_WINDOW_WIDTH);
        system->pY[particleIdx] -= WINDOW_HEIGHT * floorf(system->pY[particleIdx] * INV_WINDOW_HEIGHT);
    }
}

// thermal noise
static inline void getThermalNoise(float *jumpX, float *jumpY, float temperature)
{
    float u1;
    do
    {
        u1 = ((float)rand() / RAND_MAX);
    }
    while (u1 == 0.0f);
    float u2 = ((float)rand() / RAND_MAX);

    float magnitude = sqrtf(-2.0f * logf(u1)) * temperature;
    float angle = u2 * 2.0f * (float)M_PI;
    *jumpX += magnitude * cosf(angle) * TEMPERATURE_SCALE;
    *jumpY += magnitude * sinf(angle) * TEMPERATURE_SCALE;
}