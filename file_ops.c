#include "file_ops.h"

bool copyFile(FILE *src, FILE *dst) {
  size_t n = 0;
  char buf[BUFSIZ];
  while((n = fread(buf, sizeof(char), BUFSIZ, src)) > 0) {
    fwrite(buf, sizeof(char), n, dst);
  }
  return true;
}
