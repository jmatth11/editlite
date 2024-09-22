#ifndef EDITLITE_UNICODE_H
#define EDITLITE_UNICODE_H

#include "deps/array_template/array_template.h"
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef uint32_t code_point_t;

generate_array_template(code_point, code_point_t)

struct unicode_str {
  code_point_array points;
};

__END_DECLS

#endif
