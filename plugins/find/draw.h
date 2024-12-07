#ifndef EDITLITE_FIND_DRAW_H
#define EDITLITE_FIND_DRAW_H

#include <display.h>

void draw_background(struct display *d, int x, int y, int w, int h);
void draw_textinput(struct display *d, const char *val, const int val_size,
                    int x, int y, int w, int h);

#endif
