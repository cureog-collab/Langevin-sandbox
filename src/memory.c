#include "../include/engine.h"

#include <inttypes.h>
#include <stdlib.h>
#include <wchar.h>

particleSystem *initParticleSys(uint32_t inputCap)
{
    // malloc for the whole system struct
    particleSystem *pResultSys = malloc(sizeof(particleSystem));
    if (pResultSys == NULL)
    {
        printf("Error: failed to generate pResultSys!\n");
        return NULL;
    }
        
    // initialize the capcity and cout
    pResultSys->capacity = inputCap;
    pResultSys->count = 0;

    // malloc for pX and pY arrays
    pResultSys->pX = malloc(inputCap * sizeof(float));
    if (pResultSys->pX == NULL)
    {
        printf("Error: failed to malloc for pResultSys->pX!\n");
        free(pResultSys);
        return NULL;
    }
    pResultSys->pY = malloc(inputCap * sizeof(float));
    if (pResultSys->pY == NULL)
    {
        printf("Error: failed to malloc for pResultSys->pY!\n");
        free(pResultSys->pX);
        free(pResultSys);
        return NULL;
    }

    return pResultSys;
}

void createParticle(particleSystem *pSystem, float x, float y)
{
    uint16_t currCount = pSystem->count;
    if (currCount >= MAX_PARTICLES)
    {
        return;
    }
    pSystem->count++;
    pSystem->pX[currCount + 1] = x;
    pSystem->pY[currCount + 1] = y;
}

void annihilateParticle(particleSystem *pSystem, float x, float y)
{
    uint16_t currCount = pSystem->count;
    if (currCount == 0)
    {
        return;
    }

    // detect if the mouse clicked on any particle
    for (uint16_t i = 0; i < currCount; i++)
    {
        float dstToMouse = hypotf(pSystem->pX[i] - x, pSystem->pY[i] - y);

        // if detected a "victim"
        if (dstToMouse <= CLICK_RADIUS)
        {
            uint16_t newCount = pSystem->count - 1;
            
            pSystem->pX[i] = pSystem->pX[newCount];
            pSystem->pY[i] = pSystem->pY[newCount];
            pSystem->count--;
            
            return;
        }
    }
}

void destroyParticleSys(particleSystem *pSystem)
{
    if (pSystem == NULL)
    {
        return;
    }
    free(pSystem->pX);
    free(pSystem->pY);
    free(pSystem);
}