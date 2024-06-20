#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "command_prompt.h"
#include "config.h"
#include "display.h"
#include "key_handler.h"
#include "page.h"
#include "util.h"
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

int main(int argc, char **argv) {
  init();
  char *file = "main.c";
  if (argc > 1) {
    file = argv[1];
  }
  struct win w = {
    .window = NULL,
    .renderer = NULL,
    .height = 480,
    .width = 640
  };
  win_init(&w);
  struct config config;
  init_config(&config);
  struct command_prompt cmd;
  cmd.width = w.width - 30;
  cmd.height = 60;
  struct display d;
  d.config = config;
  if (display_init(&d, &w) != 0) {
    printf("Display could not initialize.\n");
    exit(1);
  }
  struct page_manager pm;
  if (init_page_manager(&pm) != 0) {
    printf("Page Manager could not initialize.\n");
    exit(1);
  }
  struct page test_buffer;
  if (init_page(&test_buffer) != 0) {
    printf("Page could not initialize.\n");
    exit(1);
  }
  insert_page_array(&pm.pages, test_buffer);
  if (pm.open(&pm.pages.page_data[0], file) != 0) {
    printf("Could not open file.\n");
    exit(1);
  }
  if (pm.read(&pm.pages.page_data[0]) != 0) {
    printf("Could not read file.\n");
    exit(1);
  }
  d.page_mgr = pm;

  clock_t init;
  SDL_Event e; int quit = 0;
  while(!quit) {
    init = clock();
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = 1;
      } else if (e.type == SDL_KEYDOWN) {
        handle_keydown(&d, &w, &e);
      }
    }
    SDL_SetRenderDrawColor(w.renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(w.renderer);
    display_page_render(&d, &w);
    if (d.mode == COMMAND) {
      display_command_prompt(&cmd, &d, &w);
    }
    SDL_RenderPresent(w.renderer);
    double cur_exec_time = ((clock() - init) / (double)CLOCKS_PER_SEC);
    double tick = 1000.0 / 60.0;
    if (cur_exec_time < tick) {
      double delay = tick - cur_exec_time;
      SDL_Delay(delay);
    }
  }
  display_free(&d);
  win_free(&w);
  deinit();
  return 0;
}
