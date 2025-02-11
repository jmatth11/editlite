#ifndef EDITLITE_SESSION_H
#define EDITLITE_SESSION_H

#include <stddef.h>
#include "../deps/array_template/array_template.h"

struct session {
  size_t id;
  size_t page_idx;
};

generate_array_template(session, struct session);

#endif
