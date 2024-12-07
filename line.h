#ifndef EDITLITE_LINE_H
#define EDITLITE_LINE_H

#include <stddef.h>
#include "gap_buffer.h"

struct line {
  size_t start_pos;
  size_t load_pos;
  struct gap_buffer chars;
};

int init_line(struct line *l);
void free_line(struct line *l);

#endif
