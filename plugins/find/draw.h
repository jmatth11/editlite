#ifndef EDITLITE_FIND_DRAW_H
#define EDITLITE_FIND_DRAW_H

#include "types/unicode_types.h"
#include <stddef.h>
#include <stdint.h>

struct display;
struct find_info;

void draw_background(struct display *d, int x, int y, int w, int h);
void draw_textinput(struct display *d, code_point_t *const val, const int val_size,
                    int x, int y, int w, int h);
void draw_options(struct display *d, struct find_info *op, size_t len,
                  int x, int y, int w, int h);
void draw_select_box(struct display *d, int x, int y, int w, int h);

#endif
