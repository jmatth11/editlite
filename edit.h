#ifndef EDITLITE_EDIT_H
#define EDITLITE_EDIT_H

#include <stddef.h>

enum edit_mode {
  INSERT,
  DELETE,
};

struct edits {
  enum edit_mode mode;
  size_t start_pos;
  size_t end_pos;
  char* text;
};

#endif
