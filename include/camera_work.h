#ifndef CAMERAWORK_H
#define CAMERAWORK_H

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>

#include <stdbool.h>

typedef struct {
    SDL_FPoint camPos;
    float zoom;
    bool isDragging;
} camera;

// reset the entire viewport to its original zoom scale and position
void resetCamera(camera *cam, SDL_Point originalPos);

// update the viewport
bool updateViewport(camera *cam, const SDL_Event *event);

// transforming the "virtual world" coordinate to the camera's
SDL_Rect cameraMorphRect(const camera *cam, SDL_FRect worldObj);
SDL_Point cameraMorphPoint(const camera *cam, SDL_Point worldPt);

#endif