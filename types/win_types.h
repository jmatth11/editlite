#ifndef EDITLITE_WIN_TYPES_H
#define EDITLITE_WIN_TYPES_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

struct win {
  SDL_Window *window;
  SDL_Renderer *renderer;
  int height;
  int width;
};

#endif
