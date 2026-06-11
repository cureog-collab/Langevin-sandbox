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
    *jumpX = (rand() % 2 == 0) ? *jumpX + magnitude * cosf(u2) * SCALE : *jumpX - magnitude * cosf(u2) * SCALE;
    *jumpY = (rand() % 2 == 0) ? *jumpY + magnitude * sinf(u2) * SCALE : *jumpY - magnitude * sinf(u2) * SCALE;
}