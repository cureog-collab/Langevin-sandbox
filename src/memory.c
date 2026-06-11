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