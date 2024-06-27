#ifndef EDITLITE_INSERT_MODE_H
#define EDITLITE_INSERT_MODE_H

#include "display.h"
#include "win.h"

enum insert_mode_t {
  INSERT_AT = 0,
  INSERT_AFTER,
  INSERT_BEGIN,
  INSERT_END,
};

void handle_insert_mode(struct display *d, struct win *w, SDL_Event *e);
void prepare_insert_mode(struct display *d, struct win *w, enum insert_mode_t);

#endif
