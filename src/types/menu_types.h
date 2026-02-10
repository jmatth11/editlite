#ifndef EDITLITE_MENU_TYPES_H
#define EDITLITE_MENU_TYPES_H

#include <stdbool.h>
#include <SDL2/SDL_pixels.h>

#include "array_template.h"

// forward declares
struct display;
struct win;

struct menu_item {
  void *ctx;
  const char *name;
  bool (*selected)(struct display *, void *);
  SDL_Color color;
  bool use_color;
};

generate_array_template(menu_item, struct menu_item);

struct menu {
  int x, y, w, h;
  int idx;
  int visual_offset;
  menu_item_array items;
};

#endif
