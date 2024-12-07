#ifndef EDITLITE_INSERT_MODE_H
#define EDITLITE_INSERT_MODE_H

#include "display.h"
#include "win.h"

void handle_insert_mode(struct display *d, struct win *w, SDL_Event *e);
void prepare_insert_mode(struct display *d, struct win *w);

#endif
