#include "util.h"
#include "utf8.h"
#include <stdint.h>

uint8_t* encode_to_utf8(code_point_t *arr, size_t len) {
  const size_t buf_len = code_point_to_utf8_len(arr, len);
  uint8_t* buffer = malloc(sizeof(uint8_t)*buf_len + 1);
  size_t buf_idx = 0;
  for (int idx = 0; idx < len; ++ idx) {
    buf_idx += utf8_write_code_point(buffer, buf_len, buf_idx, arr[idx]);
  }
  buffer[buf_len] = '\0';
  return buffer;
}
