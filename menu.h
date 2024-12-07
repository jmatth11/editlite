#ifndef EDITLITE_MENU_H
#define EDITLITE_MENU_H

#include <stdbool.h>

// forward declares
struct display;
struct win;
struct menu {
  int x, y, w, h;
  int idx;
};

bool menu_init(struct menu *);
bool menu_display(struct display *, struct win *);

#endif
