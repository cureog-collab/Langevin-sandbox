#include "../include/camera_work.h"
#include "../include/engine.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>

const float PANSCALE = 0.8f;
const float ZOOMSCALE = 1.1f;

const float MAXZOOM = 100.0f;
const float MINZOOM = 1.0f;

void clampCamera(camera *cam);

void resetCamera(camera *cam, SDL_Point originalPos)
{
    cam->camPos.x = originalPos.x;
    cam->camPos.y = originalPos.y;
    cam->isDragging = false;
    cam->zoom = 1.0f;
}

bool updateViewport(camera *cam, const SDL_Event *event)
{
    switch (event->type) {

        // start panning
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                cam->isDragging = true;
            }
            break;

        // release the mouse button (turn off panning mode)
        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                cam->isDragging = false;
            }
            break;
        
        // panning motion
        case SDL_MOUSEMOTION:
            if (cam->isDragging)
            {
                cam->camPos.x += PANSCALE * event->motion.xrel;
                cam->camPos.y += PANSCALE * event->motion.yrel;
                
                clampCamera(cam);
            }
            break;

        // zoom to cursor
        case SDL_MOUSEWHEEL:
        {
            float currZoom = cam->zoom;

            // get the current mouse position on the screen
            SDL_Point mousePos;
            SDL_FPoint worldPos;
            SDL_GetMouseState(&mousePos.x, &mousePos.y);
            worldPos.x = (mousePos.x - cam->camPos.x) / (currZoom);
            worldPos.y = (mousePos.y - cam->camPos.y) / (currZoom);

            // zoom in or out accordingly
            if (event->wheel.y > 0)
            {
                cam->zoom = (currZoom < MAXZOOM) ? currZoom * ZOOMSCALE : MAXZOOM;
            }
            else
            {
                cam->zoom = (currZoom > MINZOOM) ? currZoom / ZOOMSCALE : MINZOOM;
            }

            // assign the new camera position
            cam->camPos.x = mousePos.x - cam->zoom * worldPos.x;
            cam->camPos.y = mousePos.y - cam->zoom * worldPos.y;

            clampCamera(cam);

            break;
        }
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
void clampCamera(camera *cam)
{
    float minX = WINDOW_WIDTH * (1.0f - cam->zoom);
    float minY = WINDOW_HEIGHT * (1.0f - cam->zoom);

    if (cam->camPos.x < minX)
    {
        cam->camPos.x = minX;
    }
    else if (cam->camPos.x > 0.0f)
    {
        cam->camPos.x = 0.0f;
    }

    if (cam->camPos.y < minY)
    {
        cam->camPos.y = minY;
    }
    else if (cam->camPos.y > 0.0f)
    {
        cam->camPos.y = 0.0f;
    }
}