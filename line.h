#ifndef EDITLITE_LINE_H
#define EDITLITE_LINE_H

#include <stddef.h>

struct line;

int init_line(struct line *l);
void free_line(struct line *l);

#endif
