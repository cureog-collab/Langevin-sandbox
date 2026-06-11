#include "../include/engine.h"
#include <math.h>
#include <stdlib.h>

#define SCALE 1.0f

static inline void getThermalNoise(float *jumpX, float *jumpY, float temperature);

void updatePhysics(particleSystem *system, float temperature)
{
    for (int particleIdx = 0; particleIdx < system->count; particleIdx++)
    {
        float jumpX = 0;
        float jumpY = 0;

        // temperature
        getThermalNoise(&jumpX, &jumpY, temperature);

        // asign jump to the current position of the pariticle
        system->pX[particleIdx] += jumpX;
        system->pY[particleIdx] += jumpY;

        // wrap-around at edges
        system->pX[particleIdx] -= WINDOW_WIDTH * floorf(system->pX[particleIdx] / WINDOW_WIDTH);
        system->pY[particleIdx] -= WINDOW_HEIGHT * floorf(system->pY[particleIdx] / WINDOW_HEIGHT);
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
    while (u1 == 0);
    float u2 = ((float)rand() / RAND_MAX);

    float magnitude = sqrtf(-2 * logf(u1)) * temperature;
    float angle = u2 * 2 * (float)M_PI;
    *jumpX += magnitude * cosf(angle) * SCALE;
    *jumpY += magnitude * sinf(angle) * SCALE;
}