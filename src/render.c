#include "../include/engine.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool initSDLGraphics(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture)
{
    // initiate stuff
    *window = SDL_CreateWindow("Langevin simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (*window == NULL)
    {
        printf("Error: failed to generate window!\n");
        return false;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL)
    {
        printf("Error: failed to generate renderer!\n");
        return false;
    }
    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, PARTICLE_SIZE, PARTICLE_SIZE);
    if (*texture == NULL)
    {
        printf("Error: failed to genereate texture!\n");
        return false;
    }
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
    
    // paint a particle
    // malloc a buffer for color
    uint32_t *pixels = malloc(PARTICLE_SIZE * PARTICLE_SIZE * sizeof(uint32_t));
    if (pixels == NULL)
    {
        printf("Error: failed to malloc for pixels!\n");
        return false;
    }

    // find the particle's center
    float centerPos = (PARTICLE_SIZE - 1.0f) / 2.0f;
    float particleRadius = centerPos;

    // paint the whole particle
    for (int y = 0; y < PARTICLE_SIZE; y++)
    {
        for (int x = 0; x < PARTICLE_SIZE; x++)
        {
            float dst = hypotf(x - centerPos, y - centerPos);

            // hash
            int hashIndex = y * PARTICLE_SIZE + x;

            pixels[hashIndex] = (dst <= particleRadius) ? 0xF9F8F6FF : 0x00000000;
        }
    }

    // pump to VRAM and free the color buffer
    SDL_UpdateTexture(*texture, NULL, pixels, PARTICLE_SIZE * sizeof(uint32_t));
    free(pixels);
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
            verts[vIdx + 0].color = (SDL_Color){249, 248, 246, 255};
            verts[vIdx + 1].color = (SDL_Color){249, 248, 246, 255};
            verts[vIdx + 2].color = (SDL_Color){249, 248, 246, 255};
            verts[vIdx + 3].color = (SDL_Color){249, 248, 246, 255};

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
    for (uint32_t i = 0; i < system->count; i++)
    {
        uint32_t vIdx = i * 4;

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

void destroyAllSDL(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}