#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "win.h"

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("couldn't load video\n");
    exit(1);
  }
  if (TTF_Init() == -1) {
    printf("failed ttf init\n");
    exit(1);
  }
}

void deinit() {
  TTF_Quit();
  SDL_Quit();
}

int main(int argc, char **arg) {
  init();
  struct win w = {
    .window = NULL,
    .renderer = NULL,
    .height = 480,
    .width = 640
  };
  create_win(&w);
  struct display d;
  init_display(&d);
  SDL_Event e; int quit = 0;
  while(!quit) {
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = 1;
      }
    }
    SDL_SetRenderDrawColor(w.renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(w.renderer);

    SDL_RenderPresent(w.renderer);
  }
  free_win(&w);
  deinit();
  return 0;
}
