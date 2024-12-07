#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "command_prompt.h"
#include "config.h"
#include "key_handler.h"
#include "page.h"
#include "util.h"
#include "win.h"
#include "display.h"

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "couldn't load video\n");
    exit(1);
  }
  if (TTF_Init() == -1) {
    fprintf(stderr, "failed ttf init\n");
    exit(1);
  }
}

void deinit() {
  TTF_Quit();
  SDL_Quit();
}

int main(int argc, char **argv) {
  init();
  char *file = NULL;
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
  config_init(&config);
  struct command_prompt cmd;
  cmd.width = w.width - 30;
  cmd.height = 60;
  struct display d;
  d.config = config;
  if (display_init(&d, &w) != 0) {
    fprintf(stderr, "Display could not initialize.\n");
    exit(1);
  }
  struct page_manager pm;
  if (page_manager_init(&pm) != 0) {
    fprintf(stderr, "Page Manager could not initialize.\n");
    exit(1);
  }
  struct page test_buffer;
  if (!page_init(&test_buffer)) {
    fprintf(stderr, "Page could not initialize.\n");
    exit(1);
  }
  if (file != NULL) {
    test_buffer.file_name = malloc(sizeof(char) * strlen(file));
    strcpy(test_buffer.file_name, file);
  }
  if (insert_page_array(&pm.pages, test_buffer) != 0) {
    fprintf(stderr, "error inserting page into page manager.\n");
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
    if (!display_page_render(&d, &w)) {
      fprintf(stderr, "page render failed.\n");
    }
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
