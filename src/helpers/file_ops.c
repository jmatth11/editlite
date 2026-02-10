#include "file_ops.h"
#include <stdint.h>

bool copyFile(FILE *src, FILE *dst) {
  size_t n = 0;
  uint8_t buf[BUFSIZ];
  while((n = fread(buf, sizeof(uint8_t), BUFSIZ, src)) > 0) {
    fwrite(buf, sizeof(uint8_t), n, dst);
  }
  return true;
}
