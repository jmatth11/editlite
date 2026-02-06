#include "unicode.h"
#include "types/unicode_types.h"


bool unicode_str_init(struct unicode_str *str) {
  return init_code_point_array(&str->points, 2);
}

void unicode_str_free(struct unicode_str *str) {
  free_code_point_array(&str->points);
}

void utf8_buffer_free(struct utf8_buffer *buf) {
  free(buf->buffer);
  buf->buffer = NULL;
  buf->len = 0;
}
