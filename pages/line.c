#include "line.h"
#include "types/line_types.h"

#define LINE_CHAR_BUFF_SIZE 10

int init_line(struct line *l) {
  if (!gap_buffer_init(&l->chars, LINE_CHAR_BUFF_SIZE)) return 1;
  l->load_pos = 0;
  l->start_pos = 0;
  return 0;
}

void free_line(struct line *l) {
  gap_buffer_free(&l->chars);
}

