#ifndef EDITLITE_LINE_TYPES_H
#define EDITLITE_LINE_TYPES_H

#include <structures/gap_buffer.h>

struct line {
  size_t start_pos;
  size_t load_pos;
  struct gap_buffer chars;
};

#endif
