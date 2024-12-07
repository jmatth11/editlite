#ifndef EDITLITE_UNICODE_H
#define EDITLITE_UNICODE_H

#include "deps/array_template/array_template.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef uint32_t code_point_t;

generate_array_template(code_point, code_point_t)

struct unicode_str {
  code_point_array points;
};

/**
 * utf8 string wrapper.
 */
struct utf8_buffer {
  size_t len;
  uint8_t *buffer;
};

__END_DECLS

#endif
