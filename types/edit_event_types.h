#ifndef EDITLITE_EDIT_EVENT_TYPES_H
#define EDITLITE_EDIT_EVENT_TYPES_H

#include <stddef.h>
#include <time.h>

#include "types/unicode_types.h"

/**
 * Edit event type enums.
 */
enum edit_event_type {
  EE_INSERT,
  EE_DELETE,
};

/**
 * Edit Event structure.
 */
struct edit_event {
  /* the type */
  enum edit_event_type type;
  /* the character if INSERT */
  code_point_t character;
  /* the line number */
  size_t line;
  /* the column number */
  size_t col;
  /* The timestamp of the edit */
  time_t timestamp;
};

#endif
