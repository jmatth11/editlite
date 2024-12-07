#include "page.h"
#include <stdbool.h>
#include <stdio.h>

#ifndef BUFSIZ
#define BUFSIZ 8192
#endif
#define BUFFER_LIMIT BUFSIZ * 4

static char buffer[BUFSIZ];

bool page_manager_open(struct page *buf) {
  if (buf->file_name == NULL) {
    fprintf(stderr, "file name was null.\n");
    return false;
  }
  buf->fp = fopen(buf->file_name, "r+");
  if (buf->fp == NULL) {
    return false;
  }
  fseek(buf->fp, 0L, SEEK_END);
  buf->file_size = ftell(buf->fp);
  fseek(buf->fp, 0L, SEEK_SET);
  return true;
}

bool page_manager_read(struct page *buf) {
  int done = 0;
  int char_idx = 0;
  struct linked_list *cur_line = linked_list_get_end(buf->lines);
  struct line tmp_line;
  init_line(&tmp_line);
  if (cur_line->prev == NULL) {
    cur_line->value = tmp_line;
  } else {
    linked_list_append(cur_line, tmp_line);
    cur_line = cur_line->next;
  }
  bool create_new_line = false;
  while (!done) {
    size_t n = fread(buffer, sizeof(char), BUFSIZ, buf->fp);
    if (n < BUFSIZ) {
      done = 1;
    }
    for (int i = 0; i < n; ++i) {
      // create new line entry on the next go so we don't have empty lines
      // at the end of the linked list
      if (create_new_line) {
        init_line(&tmp_line);
        tmp_line.start_pos = (char_idx + 1) + buf->file_offset_pos;
        tmp_line.load_pos = tmp_line.start_pos;
        linked_list_append(cur_line, tmp_line);
        cur_line = cur_line->next;
        char_idx = 0;
        create_new_line = false;
      }
      char cur_char = buffer[i];
      gap_buffer_insert(&cur_line->value.chars, cur_char);
      if (cur_char == '\n') {
        cur_line->value.load_pos = char_idx + buf->file_offset_pos;
        create_new_line = true;
      } else {
        char_idx++;
      }
    }
    buf->file_offset_pos += n;
    if (buf->file_offset_pos >= BUFFER_LIMIT) {
      done = 1;
    }
  }
  return true;
}

bool page_init(struct page *p) {
  p->lines = (struct linked_list*)malloc(sizeof(struct linked_list));
  int err = linked_list_init(p->lines);
  p->file_size = 0;
  p->file_offset_pos = 0;
  p->col_offset = 0;
  p->row_offset = 0;
  p->file_name = NULL;
  p->fp = NULL;
  return err == 0;
}

void page_free(struct page *p) {
  linked_list_free_all(p->lines);
  if (p->file_name != NULL) {
    free(p->file_name);
    p->file_name = NULL;
  }
  if (p->fp != NULL) {
    fclose(p->fp);
    p->fp = NULL;
  }
}

int page_manager_init(struct page_manager *pm) {
  int err = init_page_array(&pm->pages, 1);
  pm->open = page_manager_open;
  pm->read = page_manager_read;
  return err;
}

void page_manager_free(struct page_manager *pm) {
  for (int i = 0; i < pm->pages.len; ++i) {
    page_free(&pm->pages.page_data[i]);
  }
  free_page_array(&pm->pages);
}
