#ifndef EDITLITE_COMMAND_PROMPT_H
#define EDITLITE_COMMAND_PROMPT_H

#include "display.h"
#include "win.h"
#include <stddef.h>

struct command_prompt {
  size_t height;
  size_t width;
  char* buf;
};

int display_command_prompt(struct command_prompt *cmd, struct display *d, struct win *w);

#endif
