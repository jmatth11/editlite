#include "page.h"
#include <stdio.h>

#ifndef BUFSIZ
#define BUFSIZ 8192
#endif
#define LINE_CHAR_BUFF_SIZE 10
#define BUFFER_LIMIT BUFSIZ * 4

static char buffer[BUFSIZ];

int page_manager_open(struct page *buf, const char *file_name) {
  buf->fp = fopen(file_name, "r+");
  if (buf->fp == NULL) {
    return 1;
  }
  fseek(buf->fp, 0L, SEEK_END);
  buf->file_size = ftell(buf->fp);
  fseek(buf->fp, 0L, SEEK_SET);
  return 0;
}

int page_manager_read(struct page *buf) {
  int done = 0;
  int char_idx = 0;
  int line_idx = buf->lines.len;
  struct line tmp_line;
  init_line(&tmp_line);
  insert_line_array(&buf->lines, tmp_line);
  while (!done) {
    size_t n = fread(buffer, sizeof(char), BUFSIZ, buf->fp);
    if (n < BUFSIZ) {
      done = 1;
    }
    for (int i = 0; i < n; ++i) {
      char cur_char = buffer[i];
      struct line *line_buf = &buf->lines.line_data[line_idx];
      gap_buffer_insert(&line_buf->chars, cur_char);
      if (cur_char == '\n') {
        line_idx++;
        line_buf->load_pos = char_idx + buf->file_offset_pos;
        init_line(&tmp_line);
        tmp_line.start_pos = (char_idx + 1) + buf->file_offset_pos;
        tmp_line.load_pos = tmp_line.start_pos;
        insert_line_array(&buf->lines, tmp_line);
        char_idx = 0;
      } else {
        char_idx++;
      }
    }
    buf->file_offset_pos += n;
    if (buf->file_offset_pos >= BUFFER_LIMIT) {
      done = 1;
    }
  }
  return 0;
}

int init_line(struct line *l) {
  if (!gap_buffer_init(&l->chars, LINE_CHAR_BUFF_SIZE)) return 1;
  l->load_pos = 0;
  l->start_pos = 0;
  return 0;
}

void free_line(struct line *l) {
  gap_buffer_free(&l->chars);
}

int init_page(struct page *p) {
  int err = init_line_array(&p->lines, LINE_CHAR_BUFF_SIZE);
  p->file_size = 0;
  p->file_offset_pos = 0;
  p->col_offset = 0;
  p->row_offset = 0;
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
  int err = init_page_array(&pm->pages, 1);
  pm->open = page_manager_open;
  pm->read = page_manager_read;
  return err;
}

void free_page_manager(struct page_manager *pm) {
  for (int i = 0; i < pm->pages.len; ++i) {
    free_page(&pm->pages.page_data[i]);
  }
  free_page_array(&pm->pages);
}
