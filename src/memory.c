#include "../include/engine.h"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <wchar.h>

// particle =================================================================================================
particleSystem *initParticleSys(void)
{
    // malloc for the whole system struct
    particleSystem *pResultSys = malloc(sizeof(particleSystem));
    if (pResultSys == NULL)
    {
        printf("Error: failed to generate pResultSys!\n");
        return NULL;
    }
        
    // initialize count
    pResultSys->count = 0;

    // malloc for pX and pY arrays
    pResultSys->pX = malloc(MAX_PARTICLES * sizeof(float));
    if (pResultSys->pX == NULL)
    {
        printf("Error: failed to malloc for pResultSys->pX!\n");
        free(pResultSys);
        return NULL;
    }
    pResultSys->pY = malloc(MAX_PARTICLES * sizeof(float));
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
    pSystem->pX[currCount] = x;
    pSystem->pY[currCount] = y;
    pSystem->count++;
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
// particle =================================================================================================

//defect ===================================================================================================
defectSystem* initDefectSys(void)
{
    defectSystem *pResultSys = malloc(sizeof(defectSystem));
    if (pResultSys == NULL)
    {
        printf("Error: failed to initiate defect system!\n");
        return NULL;
    }

    pResultSys->pX = malloc(MAX_DEFECTS * sizeof(float));
    if (pResultSys->pX == NULL)
    {
        printf("Error: failed to malloc for pX of defect system!\n");
        free(pResultSys);
        return NULL;
    }
    pResultSys->pY = malloc(MAX_DEFECTS * sizeof(float));
    if (pResultSys->pY == NULL)
    {
        printf("Error: failed to malloc for pY of defect system!\n");
        free(pResultSys->pX);
        free(pResultSys);
        return NULL;
    }
    pResultSys->count = 0;

    return pResultSys;
}

forceGrid* initForceGrid(const defectSystem *pDefectSys)
{
    forceGrid *pResultSys = malloc(sizeof(forceGrid));
    if (pResultSys == NULL)
    {
        printf("Error: failed to initiate force grid!\n");
        return NULL;
    }

    // numbers of cols and rows
    pResultSys->cols = WINDOW_WIDTH * INV_GRID_CELL_SIZE;
    pResultSys->rows = WINDOW_HEIGHT * INV_GRID_CELL_SIZE;
    uint32_t totNodes = pResultSys->cols * pResultSys->rows;

    // allocate memory for storing force information at each node
    pResultSys->forceX = calloc(totNodes, sizeof(float));
    if (pResultSys->forceX == NULL)
    {
        printf("Error: failed to calloc for pResultSys->forceX!\n");
        free(pResultSys);
        return NULL;
    }
    pResultSys->forceY = calloc(totNodes, sizeof(float));
    if (pResultSys->forceY == NULL)
    {
        printf("Error: failed to calloc for pResultSys->forceY!\n");
        free(pResultSys->forceX);
        free(pResultSys);
        return NULL;
    }

    // initiate force magnitude to 0
    memset(pResultSys->forceX, 0.0f, totNodes * sizeof(float));
    memset(pResultSys->forceY, 0.0f, totNodes * sizeof(float));

    // asign values of force magnitude at each node
    if (pDefectSys != NULL && pDefectSys->count > 0)
    {
        updateForceGrid(pResultSys, pDefectSys);
    }

    return pResultSys;
}

void updateForceGrid(forceGrid *pforceGrid, const defectSystem *pDefectSys)
{
    for (uint32_t row = 0; row < pforceGrid->rows; row++)
    {
        for (uint32_t col = 0; col < pforceGrid->cols; col++)
        {
            float coordY = row * GRID_CELL_SIZE;
            float coordX = col * GRID_CELL_SIZE;

            // prepare sum
            float addForceX = 0.0f;
            float addForceY = 0.0f;

            // loop through evey single defect on the map
            for (int defectIdx = 0; defectIdx < pDefectSys->count; defectIdx++)
            {
                float dx = pDefectSys->pX[defectIdx] - coordX;
                float dy = pDefectSys->pY[defectIdx] - coordY;
                float forceFactor = DEFECT_FORCE * expf(-DEFECT_ALPHA * (dx * dx + dy * dy));

                addForceX += dx * forceFactor;
                addForceY += dy * forceFactor;
            }

            // hash
            int gridIdx = row * pforceGrid->cols + col;
            pforceGrid->forceX[gridIdx] = fmaxf(-MAX_DEFECT_FORCE, fminf(MAX_DEFECT_FORCE, addForceX));
            pforceGrid->forceY[gridIdx] = fmaxf(-MAX_DEFECT_FORCE, fminf(MAX_DEFECT_FORCE, addForceY));
        }
    }
}

void destroyDefectSys(defectSystem *pDefectSys)
{
    if (pDefectSys == NULL)
    {
        return;
    }
    free(pDefectSys->pX);
    free(pDefectSys->pY);
    free(pDefectSys);
}
void destroyForceGrid(forceGrid *grid)
{
    if (grid == NULL)
    {
        return;
    }
    free(grid->forceX);
    free(grid->forceY);
    free(grid);
}

void createDefect(forceGrid *pForceGrid, defectSystem *pDefectSys, float x, float y)
{
    uint16_t currCount = pDefectSys->count;
    if (currCount == MAX_DEFECTS)
    {
        return;
    }
    pDefectSys->pX[currCount] = x;
    pDefectSys->pY[currCount] = y;
    pDefectSys->count++;
    updateForceGrid(pForceGrid, pDefectSys);
}
void annihilateDefect(forceGrid *pForceGrid, defectSystem *pDefectSys, float x, float y)
{
    uint16_t currCount = pDefectSys->count;
    if (currCount == 0)
    {
        return;
    }

    // detect if the mouse clicked on any particle
    for (uint16_t i = 0; i < currCount; i++)
    {
        float dstToMouse = hypotf(pDefectSys->pX[i] - x, pDefectSys->pY[i] - y);

        // if detected a "victim"
        if (dstToMouse <= CLICK_RADIUS)
        {
            uint16_t newCount = pDefectSys->count - 1;
            
            pDefectSys->pX[i] = pDefectSys->pX[newCount];
            pDefectSys->pY[i] = pDefectSys->pY[newCount];
            pDefectSys->count--;
            updateForceGrid(pForceGrid, pDefectSys);
            return;
        }
    }
}
// defect ===================================================================================================