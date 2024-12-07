#ifndef EDITLITE_INSERT_MODE_H
#define EDITLITE_INSERT_MODE_H

#include <SDL2/SDL_events.h>

struct display;

enum insert_mode_t {
  INSERT_AT = 0,
  INSERT_AFTER,
  INSERT_BEGIN,
  INSERT_END,
};

void handle_insert_mode(struct display *d, SDL_Event *e);
void handle_input_mode(struct display *d, SDL_Event *e);
void prepare_insert_mode(struct display *d, enum insert_mode_t);

#endif
