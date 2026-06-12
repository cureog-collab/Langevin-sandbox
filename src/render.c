#include "../include/engine.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Texture* paintGradientTexture(SDL_Renderer *renderer, uint8_t targetSize, uint8_t r, uint8_t g, uint8_t b);

bool initSDLGraphics(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **particleTexture, SDL_Texture **defectTexture)
{
    // initiate window and renderer
    *window = SDL_CreateWindow("Langevin simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (*window == NULL)
    {
        printf("Error: failed to generate window!\n");
        return false;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL)
    {
        SDL_DestroyWindow(*window);
        printf("Error: failed to generate renderer!\n");
        return false;
    }

    // paint particle texture
    *particleTexture = paintGradientTexture(*renderer, PARTICLE_SIZE, 10, 25, 30);
    if (*particleTexture == NULL)
    {
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        printf("Error: failed to generate particle texture!\n");
        return false;
    }

    // paint defect texture
    *defectTexture = paintGradientTexture(*renderer, DEFECT_SIZE, 230, 230, 247);
    if (*defectTexture == NULL)
    {
        SDL_DestroyTexture(*particleTexture);
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        printf("Error: failed to generate defect texture!\n");
        return false;
    }

    return true;
}

bool renderParticles(SDL_Renderer *renderer, SDL_Texture *texture, particleSystem *system, camera *cam)
{
    static SDL_Vertex verts [MAX_PARTICLES * 4];
    static int indices[MAX_PARTICLES * 6];
    static bool isInitialized = false;

    if (isInitialized == false)
    {
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            int vIdx = i * 4; 
            int iIdx = i * 6;

            // color
            verts[vIdx + 0].color = (SDL_Color){255, 255, 255, 255};
            verts[vIdx + 1].color = (SDL_Color){255, 255, 255, 255};
            verts[vIdx + 2].color = (SDL_Color){255, 255, 255, 255};
            verts[vIdx + 3].color = (SDL_Color){255, 255, 255, 255};

            // coordinates
            verts[vIdx + 0].tex_coord = (SDL_FPoint){0.0f, 0.0f}; // top-left
            verts[vIdx + 1].tex_coord = (SDL_FPoint){1.0f, 0.0f}; // top-right
            verts[vIdx + 2].tex_coord = (SDL_FPoint){1.0f, 1.0f}; // bot-right
            verts[vIdx + 3].tex_coord = (SDL_FPoint){0.0f, 1.0f}; // bot-left

            // 1st triangle
            indices[iIdx + 0] = vIdx + 0;
            indices[iIdx + 1] = vIdx + 1;
            indices[iIdx + 2] = vIdx + 2;

            // 2nd triangle
            indices[iIdx + 3] = vIdx + 0;
            indices[iIdx + 4] = vIdx + 2;
            indices[iIdx + 5] = vIdx + 3;
        }
        isInitialized = true;
    }

    // move the particles
    for (uint16_t i = 0; i < system->count; i++)
    {
        uint16_t vIdx = i * 4;

        float worldX = system->pX[i];
        float worldY = system->pY[i];

        float screenX = (worldX * cam->zoom) + cam->camPos.x;
        float screenY = (worldY * cam->zoom) + cam->camPos.y;
        float screenRadius = (PARTICLE_SIZE / 2.0f) * cam->zoom;

        verts[vIdx + 0].position = (SDL_FPoint){ screenX - screenRadius, screenY - screenRadius}; // top-left
        verts[vIdx + 1].position = (SDL_FPoint){ screenX + screenRadius, screenY - screenRadius}; // top-right
        verts[vIdx + 2].position = (SDL_FPoint){ screenX + screenRadius, screenY + screenRadius}; // bot-right
        verts[vIdx + 3].position = (SDL_FPoint){ screenX - screenRadius, screenY + screenRadius}; // bot-left
    }

    int vertexCount = system->count * 4;
    int indexCount  = system->count * 6;

    SDL_RenderGeometry(renderer, texture, verts, vertexCount, indices, indexCount);
    
    return true;
}

bool renderDefects(SDL_Renderer *renderer, SDL_Texture *texture, defectSystem *system, camera *cam)
{
    static SDL_Vertex verts[MAX_DEFECTS * 4];
    static int indices[MAX_DEFECTS * 6];
    static bool isInitialized = false;

    if (!isInitialized)
    {
        for (int i = 0; i < MAX_DEFECTS; i++)
        {
            int vIdx = i * 4; 
            int iIdx = i * 6;

            verts[vIdx + 0].color = (SDL_Color){255, 255, 255, 255};
            verts[vIdx + 1].color = (SDL_Color){255, 255, 255, 255};
            verts[vIdx + 2].color = (SDL_Color){255, 255, 255, 255};
            verts[vIdx + 3].color = (SDL_Color){255, 255, 255, 255};

            verts[vIdx + 0].tex_coord = (SDL_FPoint){0.0f, 0.0f}; 
            verts[vIdx + 1].tex_coord = (SDL_FPoint){1.0f, 0.0f}; 
            verts[vIdx + 2].tex_coord = (SDL_FPoint){1.0f, 1.0f}; 
            verts[vIdx + 3].tex_coord = (SDL_FPoint){0.0f, 1.0f}; 

            indices[iIdx + 0] = vIdx + 0;
            indices[iIdx + 1] = vIdx + 1;
            indices[iIdx + 2] = vIdx + 2;

            indices[iIdx + 3] = vIdx + 0;
            indices[iIdx + 4] = vIdx + 2;
            indices[iIdx + 5] = vIdx + 3;
        }
        isInitialized = true;
    }

    for (int i = 0; i < system->count; i++)
    {
        int vIdx = i * 4;

        float worldX = system->pX[i];
        float worldY = system->pY[i];

        float screenX = (worldX * cam->zoom) + cam->camPos.x;
        float screenY = (worldY * cam->zoom) + cam->camPos.y;
        float screenRadius = (DEFECT_SIZE / 2.0f) * cam->zoom;

        verts[vIdx + 0].position = (SDL_FPoint){ screenX - screenRadius, screenY - screenRadius}; 
        verts[vIdx + 1].position = (SDL_FPoint){ screenX + screenRadius, screenY - screenRadius}; 
        verts[vIdx + 2].position = (SDL_FPoint){ screenX + screenRadius, screenY + screenRadius}; 
        verts[vIdx + 3].position = (SDL_FPoint){ screenX - screenRadius, screenY + screenRadius}; 
    }

    SDL_RenderGeometry(renderer, texture, verts, system->count * 4, indices, system->count * 6);
    return true;
}

void destroyAllSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *particleTexture, SDL_Texture *defectTexture)
{
    SDL_DestroyTexture(defectTexture);
    SDL_DestroyTexture(particleTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

// helper
// paint gradient circle
static SDL_Texture *paintGradientTexture(SDL_Renderer *renderer, uint8_t targetSize, uint8_t r, uint8_t g, uint8_t b)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_STREAMING, targetSize, targetSize);
    if (texture == NULL)
    {
        printf("Error: failed to create texture for painting gradient circles!\n");
        return NULL;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    uint32_t *pixels = malloc(targetSize * targetSize * sizeof(uint32_t));
    if (pixels == NULL)
    {
        SDL_DestroyTexture(texture);
        printf("Error: failed to malloc for pixels!\n");
        return NULL;
    }

    float centerPos = (targetSize - 1.0f) / 2.0f;
    float radius = centerPos;

    // paint each pixel inside the texture
    for (int y = 0; y < targetSize; y++)
    {
        for (int x = 0; x < targetSize; x++)
        {
            float dx = x - centerPos;
            float dy = y - centerPos;
            int pixelIndex = y * targetSize + x;
            float dist = hypotf(dx, dy);

            if (dist <= radius)
            {
                float alphaFactor = 1.0f - (dist / radius);
                uint8_t alpha = (uint8_t)(255.0f * powf(alphaFactor, 1.8f));
                pixels[pixelIndex] = (r << 24) | (g << 16) | (b << 8) | alpha;
            }
            else
            {
                pixels[pixelIndex] = 0x00000000;
            }
        }
    }

    SDL_UpdateTexture(texture, NULL, pixels, targetSize * sizeof(uint32_t));
    free(pixels);
    return texture;
}