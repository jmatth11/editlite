#ifndef EDITLITE_PAGE_H
#define EDITLITE_PAGE_H

#include <stdio.h>
#include <stddef.h>
#include "deps/array_template/array_template.h"

generate_array_template(char, char)

struct line {
  size_t start_pos;
  size_t load_pos;
  char_array chars;
};

int init_line(struct line *l);
void free_line(struct line *l);

generate_array_template(line, struct line)

struct page {
  FILE *fp;
  size_t file_offset_pos;
  size_t file_size;
  int col_offset;
  int row_offset;
  line_array lines;
};

int init_page(struct page* p);
void free_page(struct page* p);

generate_array_template(page, struct page)

struct page_manager {
  page_array pages;
  int (*open)(struct page* buf, const char* file_name);
  int (*read)(struct page* buf);
};

int init_page_manager(struct page_manager* pm);
void free_page_manager(struct page_manager* pm);

#endif
