#ifndef EDITLITE_BUF_H
#define EDITLITE_BUF_H

#include <SDL2/SDL_rect.h>
#include "deps/array_template/array_template.h"

struct char_info {
  char c;
  SDL_Rect size;
};

generate_array_template(char_buffer, char_info)

#endif
