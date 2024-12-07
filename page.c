#include "page.h"

int page_manager_read(struct page *buf, const char *file_name) {
  buf->fp = fopen(file_name, "r+");
  if (buf->fp == NULL) {
    return 1;
  }
  int done = 0;
  int char_idx = 0;
  int line_idx = 0;
  while (!done) {
    char cur_char = fgetc(buf->fp);
    // TODO load in data.
  }
  return 0;
}

int init_line(struct line *l) {
  int err = init_char_array(&l->buffer, 4096);
  l->load_pos = 0;
  l->start_pos = 0;
  return err;
}

int init_page(struct page *p) {
  int err = init_line_array(&p->lines, 4096);
  p->load_pos = 0;
  p->fp = NULL;
  return err;
}

int init_page_manager(struct page_manager *pm) {
  int err = init_page_array(&pm->buf, 1);
  pm->read_file = page_manager_read;
  return err;
}
