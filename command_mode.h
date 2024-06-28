#ifndef EDITLITE_COMMAN_MODE_H
#define EDITLITE_COMMAN_MODE_H

#include "display.h"
#include "win.h"

void handle_command_mode(struct display *d, struct win *w, SDL_Event *e);
void prepare_command_mode(struct display *d);
void leaving_command_mode(struct display *d);

#endif
