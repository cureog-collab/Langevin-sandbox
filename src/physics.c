#include "../include/engine.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#define TIMESTEP 1.0f
#define MAX_JUMP_PER_FRAME 5.0f

#define TEMPERATURE_SCALE 0.3f
#define LORENTZ_SCALE 2.0f
#define P_TO_P_SCALE_SML_DIST 1.53f
#define P_TO_P_SCALE_MED_DIST 300.0f
#define P_TO_P_SCALE_BIG_DIST 125.8f

const float INV_WINDOW_WIDTH = 1.0f / WINDOW_WIDTH;
const float INV_WINDOW_HEIGHT = 1.0f / WINDOW_HEIGHT;

static inline void getThermalNoise(float *jumpX, float *jumpY, float temperature);
static inline void getGridForce(const forceGrid *pForceGrid, float x, float y, float *actingFx, float *actingFy);
static inline void getPtoPInteraction(float dist, float dx, float dy, float *forceX, float *forceY);
static inline void clampJump(float *jumpX, float *jumpY);

void updatePhysics(particleSystem *particleSys, forceGrid *defectForceGrid, const simConfig *config)
{
    float totalJumpX[MAX_PARTICLES] = {0.0f};
    float totalJumpY[MAX_PARTICLES] = {0.0f};

    // particle-particle interaction
    for (int i = 0; i < particleSys->count; i++)
    {
        for (int j = i + 1; j < particleSys->count; j++)
        {
            // distance between the two particles (wrap-around)
            float dx = particleSys->pX[i] - particleSys->pX[j];
            float dy = particleSys->pY[i] - particleSys->pY[j];
            dx -= WINDOW_WIDTH * roundf(dx * INV_WINDOW_WIDTH);
            dy -= WINDOW_HEIGHT * roundf(dy * INV_WINDOW_HEIGHT);
            float dist = hypotf(dx, dy);

            float fx = 0.0f;
            float fy = 0.0f;
            getPtoPInteraction(dist, dx, dy, &fx, &fy);

            // accumulate the interaction force into particles' jumps
            totalJumpX[i] += fx;
            totalJumpY[i] += fy;
            totalJumpX[j] -= fx;
            totalJumpY[j] -= fy;
        }
    }

    for (int particleIdx = 0; particleIdx < particleSys->count; particleIdx++)
    {
        float temperature = config->temperature;
        float jumpX = totalJumpX[particleIdx];
        float jumpY = totalJumpY[particleIdx];

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
        
        // clamp jump distance
        clampJump(&jumpX, &jumpY);

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

static inline void getPtoPInteraction(float dist, float dx, float dy, float *forceX, float *forceY)
{
    float invDist = 1.0f / dist;
    if (dist > CUTOF_P_TO_P_INTERACTION_LEN)
    {
        return;
    }
    else if (dist < COHERENT_LEN)
    {
        // F ~ r
        *forceX += P_TO_P_SCALE_SML_DIST * dx;
        *forceY += P_TO_P_SCALE_SML_DIST * dy;
    }
    else if (dist < LONDON_LEN)
    {
        // F ~ 1/r
        *forceX += P_TO_P_SCALE_MED_DIST * dx * invDist * invDist;
        *forceY += P_TO_P_SCALE_MED_DIST * dy * invDist * invDist;
    }
    else
    {
        // F ~ exp(-r/ lambda) / sqrtf(r)
        float magF = P_TO_P_SCALE_BIG_DIST * sqrtf(invDist) * expf(-dist / LONDON_LEN);
        *forceX += magF * dx * invDist;
        *forceY += magF * dy * invDist;
    }
}

static inline void clampJump(float *jumpX, float *jumpY)
{
    float jumpMag = hypotf(*jumpX, *jumpY);
    if (jumpMag > MAX_JUMP_PER_FRAME)
    {
        float rescaleJump = MAX_JUMP_PER_FRAME / jumpMag;
        *jumpX *= rescaleJump;
        *jumpY *= rescaleJump;
    }

    *jumpX *= TIMESTEP;
    *jumpY *= TIMESTEP;
}