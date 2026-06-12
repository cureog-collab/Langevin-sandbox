#include "../include/engine.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#define TEMPERATURE_SCALE 0.8f
#define LORENTZ_SCALE 2.0f

const float INV_WINDOW_WIDTH = 1.0f / WINDOW_WIDTH;
const float INV_WINDOW_HEIGHT = 1.0f / WINDOW_HEIGHT;

static inline void getThermalNoise(float *jumpX, float *jumpY, float temperature);
static inline void getGridForce(const forceGrid *pForceGrid, float x, float y, float *actingFx, float *actingFy);

void updatePhysics(particleSystem *particleSys, forceGrid *defectForceGrid, const simConfig *config)
{
    for (int particleIdx = 0; particleIdx < particleSys->count; particleIdx++)
    {
        float temperature = config->temperature;
        float jumpX = 0.0f;
        float jumpY = 0.0f;

        // temperature
        getThermalNoise(&jumpX, &jumpY, temperature);

        // Lorentz force
        jumpX += config->Jy * LORENTZ_SCALE;
        jumpY -= config->Jx * LORENTZ_SCALE;

        // defect force
        if (defectForceGrid != NULL)
        {
            float defectFx = 0;
            float defectFy = 0;
            getGridForce(defectForceGrid, particleSys->pX[particleIdx], particleSys->pY[particleIdx],
                &defectFx, &defectFy);
            jumpX += defectFx;
            jumpY += defectFy;
        }

        // asign jump to the current position of the pariticle
        particleSys->pX[particleIdx] += jumpX;
        particleSys->pY[particleIdx] += jumpY;

        // wrap-around at edges
        particleSys->pX[particleIdx] -= WINDOW_WIDTH * floorf(particleSys->pX[particleIdx] * INV_WINDOW_WIDTH);
        particleSys->pY[particleIdx] -= WINDOW_HEIGHT * floorf(particleSys->pY[particleIdx] * INV_WINDOW_HEIGHT);
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

// defect force
static inline void getGridForce(const forceGrid *pForceGrid, float x, float y, float *actingFx, float *actingFy)
{
    float gridX = x * INV_GRID_CELL_SIZE;
    float gridY = y * INV_GRID_CELL_SIZE;

    // nearest top-left node
    uint16_t col0 = (uint16_t)gridX;
    uint16_t row0 = (uint16_t)gridY;

    // how far away the particle is to that node, calculated proportional to GRID_CELL_SIZE
    float far0X = gridX - (float)col0;
    float far0Y = gridY - (float)row0;

    // bot-right node (wrap-around)
    uint16_t col1 = (col0 + 1) % pForceGrid->cols;
    uint16_t row1 = (row0 + 1) % pForceGrid->rows;
    col0 = col0 % pForceGrid->cols;
    row0 = row0 % pForceGrid->rows;

    // hash the four nearest nodes enclosing the particle
    int idx00 = row0 * pForceGrid->cols + col0; // top-left
    int idx10 = row0 * pForceGrid->cols + col1; // top-right (assuming it exists)
    int idx11 = row1 * pForceGrid->cols + col1; // bot-right (assuming it exists)
    int idx01 = row1 * pForceGrid->cols + col0; // bot-left (assuming it exists)

    // bilinear interpolation for acting force along X-axis
    float fx_top = pForceGrid->forceX[idx00] * (1.0f - far0X) + pForceGrid->forceX[idx10] * far0X;
    float fx_bot = pForceGrid->forceX[idx01] * (1.0f - far0X) + pForceGrid->forceX[idx11] * far0X;
    *actingFx = fx_top * (1.0f - far0Y) + fx_bot * far0Y;

    // bilinear interpolation for acting force along Y-axis
    float fy_top = pForceGrid->forceY[idx00] * (1.0f - far0X) + pForceGrid->forceY[idx10] * far0X;
    float fy_bot = pForceGrid->forceY[idx01] * (1.0f - far0X) + pForceGrid->forceY[idx11] * far0X;
    *actingFy = fy_top * (1.0f - far0Y) + fy_bot * far0Y;
}