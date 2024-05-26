#ifndef EDITLITE_KEY_HANDLER_H
#define EDITLITE_KEY_HANDLER_H

#include "display.h"
#include <stdint.h>

void handle_simple_keypresses(struct display *d, struct win *w, SDL_Event *e);
void handle_state_keypresses(struct display *d, struct win *w, SDL_Event *e);

#endif
