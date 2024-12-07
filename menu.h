#ifndef EDITLITE_MENU_H
#define EDITLITE_MENU_H

#include <stdbool.h>
#include "deps/array_template/array_template.h"

// forward declares
struct display;
struct win;

struct menu_item {
  void *ctx;
  const char *name;
  bool (*selected)(struct display *, void *);
};

generate_array_template(menu_item, struct menu_item);

struct menu {
  int x, y, w, h;
  int idx;
  menu_item_array items;
};

bool menu_init(struct menu *);
bool menu_display(struct display *);
void menu_free(struct menu *);

#endif
