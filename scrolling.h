#ifndef EDITLITE_SCROLLING_H
#define EDITLITE_SCROLLING_H

#include "display.h"

void handle_row_scroll(struct display *d, struct display_dim dim);
void handle_col_scroll(struct display *d, struct display_dim dim);

#endif
