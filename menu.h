#ifndef EDITLITE_MENU_H
#define EDITLITE_MENU_H

#include <stdbool.h>

struct display;
struct menu;

bool menu_init(struct menu *);
bool menu_display(struct display *);
void menu_free(struct menu *);

#endif
