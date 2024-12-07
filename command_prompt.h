#ifndef EDITLITE_COMMAND_PROMPT_H
#define EDITLITE_COMMAND_PROMPT_H

#include <stddef.h>
#include "display.h"
#include "win.h"
#include "gap_buffer.h"
#include "page.h"

bool command_prompt_init(struct page *p);
bool command_prompt_display(struct display *d, struct win *w);
void command_prompt_free(struct page *p);

#endif
