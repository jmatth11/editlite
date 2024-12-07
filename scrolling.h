#ifndef EDITLITE_SCROLLING_H
#define EDITLITE_SCROLLING_H

#include "types/size_types.h"

struct display;

void handle_row_scroll(struct display *d, struct display_dim dim);
void handle_col_scroll(struct display *d, struct display_dim dim);

#endif
