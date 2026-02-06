#ifndef EDITLITE_CURSOR_TYPES_H
#define EDITLITE_CURSOR_TYPES_H

#include "size_types.h"

struct cursor {
  struct display_dim pos;
  struct display_dim screen_pos;
};

#endif
