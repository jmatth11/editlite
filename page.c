#include "page.h"
#include <stdio.h>

#define PAGE_MANAGER_BUFF_SIZE 4096
#define LINE_CHAR_BUFF_SIZE 10
#define LINE_BUFFER 100

static char buffer[PAGE_MANAGER_BUFF_SIZE];

int page_manager_read(struct page *buf, const char *file_name) {
  buf->fp = fopen(file_name, "r+");
  if (buf->fp == NULL) {
    return 1;
  }
  int done = 0;
  int char_idx = 0;
  int line_idx = 0;
  while (!done) {
    size_t n = fread(buffer, sizeof(char), PAGE_MANAGER_BUFF_SIZE, buf->fp);
    if (n < PAGE_MANAGER_BUFF_SIZE) {
      done = 1;
    }
    if (buf->lines.len == 0) {
      struct line tmp_line;
      init_line(&tmp_line);
      insert_line_array(&buf->lines, tmp_line);
    }
    for (int i = 0; i < n; ++i) {
      char cur_char = buffer[i];
      if (cur_char == '\n') {
        line_idx++;
        struct line tmp_line;
        init_line(&tmp_line);
        insert_line_array(&buf->lines, tmp_line);
        char_idx = 0;
      }
      // TODO would like to handle dynamically loading longer lines so
      // they aren't loaded all at once.
      if (line_idx >= LINE_BUFFER) {
        done = 1;
        break;
      }
      struct line *line_buf = &buf->lines.line_data[line_idx];
      insert_char_array(&line_buf->buffer, cur_char);
      char_idx++;
    }
  }
  return 0;
}

int init_line(struct line *l) {
  int err = init_char_array(&l->buffer, LINE_CHAR_BUFF_SIZE);
  l->load_pos = 0;
  l->start_pos = 0;
  return err;
}

void free_line(struct line *l) {
  free_char_array(&l->buffer);
}

int init_page(struct page *p) {
  int err = init_line_array(&p->lines, LINE_CHAR_BUFF_SIZE);
  p->load_pos = 0;
  p->fp = NULL;
  return err;
}

void free_page(struct page *p) {
  for (int i = 0; i < p->lines.len; ++i) {
    free_line(&p->lines.line_data[i]);
  }
  free_line_array(&p->lines);
}

int init_page_manager(struct page_manager *pm) {
  int err = init_page_array(&pm->buf, 1);
  pm->read_file = page_manager_read;
  return err;
}

void free_page_manager(struct page_manager *pm) {
  for (int i = 0; i < pm->buf.len; ++i) {
    free_page(&pm->buf.page_data[i]);
  }
  free_page_array(&pm->buf);
}
