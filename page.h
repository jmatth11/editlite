#ifndef EDITLITE_PAGE_H
#define EDITLITE_PAGE_H

#include <stdio.h>
#include <stddef.h>
#include "deps/array_template/array_template.h"

generate_array_template(char, char)

struct line {
  size_t start_pos;
  size_t load_pos;
  char_array buffer;
};

generate_array_template(line, struct line)

struct page {
  FILE *fp;
  size_t load_pos;
  line_array lines;
};

struct page_manager {
  struct page buf;
  int (*read_file)(struct page buf, const char* file_name);
};

#endif
