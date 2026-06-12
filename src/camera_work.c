#include "../include/camera_work.h"
#include "../include/engine.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_scancode.h>

#define MOVECAMBYKEYSPEED 15.0f

#define PANSCALE 0.8f
#define ZOOMSCALE 1.02f
#define INVZOOMSCALE 1.0f / ZOOMSCALE

#define MAXZOOM 100.0f
#define MINZOOM 1.0f

void clampCamera(camera *cam);

static inline void applyZoom(camera *cam, float newZoom, int mouseX, int mouseY);

void moveCameraByKeys(camera *cam)
{
    const uint8_t *keystate = SDL_GetKeyboardState(NULL);
    
    // Y-axis
    if (keystate[SDL_SCANCODE_W] | keystate[SDL_SCANCODE_UP])
    {
        cam->camPos.y += MOVECAMBYKEYSPEED;

        clampCamera(cam);
    }
    if (keystate[SDL_SCANCODE_S] | keystate[SDL_SCANCODE_DOWN])
    {
        cam->camPos.y -= MOVECAMBYKEYSPEED;

        clampCamera(cam);
    }

    // X-axis
    if (keystate[SDL_SCANCODE_A] | keystate[SDL_SCANCODE_LEFT])
    {
        cam->camPos.x += MOVECAMBYKEYSPEED;

        clampCamera(cam);
    }
    if (keystate[SDL_SCANCODE_D] | keystate[SDL_SCANCODE_RIGHT])
    {
        cam->camPos.x -= MOVECAMBYKEYSPEED;

        clampCamera(cam);
    }

    // zoom
    if (keystate[SDL_SCANCODE_E] | keystate[SDL_SCANCODE_Q])
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY); // Chỉ gọi chuột khi THỰC SỰ có bấm phím

        float scale = keystate[SDL_SCANCODE_E] ? ZOOMSCALE : INVZOOMSCALE;
        applyZoom(cam, cam->zoom * scale, mouseX, mouseY);
    }
    else
    {
        clampCamera(cam);
    }
}

void resetCamera(camera *cam, SDL_Point originalPos)
{
    cam->camPos.x = originalPos.x;
    cam->camPos.y = originalPos.y;
    cam->zoom = 1.0f;
}

bool updateViewport(camera *cam, const SDL_Event *event)
{
    if (event->type == SDL_MOUSEWHEEL)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        float scale = (event->wheel.y > 0) ? ZOOMSCALE : INVZOOMSCALE;
        applyZoom(cam, cam->zoom * scale, mouseX, mouseY);
    }
    
    return true;
}

SDL_Rect cameraMorphRect(const camera *cam, SDL_FRect worldObj)
{
    return (SDL_Rect) {
        (int)(worldObj.x * cam->zoom + cam->camPos.x),
        (int)(worldObj.y * cam->zoom + cam->camPos.y),
        (int)(worldObj.w * cam->zoom),
        (int)(worldObj.h * cam->zoom)
    };
}

SDL_Point cameraMorphPoint(const camera *cam, SDL_Point worldPt)
{
    return (SDL_Point) {
        (int)(worldPt.x * cam->zoom + cam->camPos.x),
        (int)(worldPt.y * cam->zoom + cam->camPos.y),
    };
}

// helper
static inline void applyZoom(camera *cam, float newZoom, int mouseX, int mouseY)
{
    newZoom = fmaxf(MINZOOM, fminf(MAXZOOM, newZoom));

    if (newZoom == cam->zoom)
    {
        return;
    }

    float worldX = (mouseX - cam->camPos.x) / cam->zoom;
    float worldY = (mouseY - cam->camPos.y) / cam->zoom;

    cam->zoom = newZoom;

    cam->camPos.x = mouseX - cam->zoom * worldX;
    cam->camPos.y = mouseY - cam->zoom * worldY;

    clampCamera(cam);
}

void clampCamera(camera *cam)
{
    float minX = WINDOW_WIDTH * (1.0f - cam->zoom);
    float minY = WINDOW_HEIGHT * (1.0f - cam->zoom);

    cam->camPos.x = fminf(0.0f, fmaxf(minX, cam->camPos.x));
    cam->camPos.y = fminf(0.0f, fmaxf(minY, cam->camPos.y));
}