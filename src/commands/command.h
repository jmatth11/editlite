#ifndef EDITLITE_COMMAND_PROMPT_H
#define EDITLITE_COMMAND_PROMPT_H

#include <stdbool.h>

struct command;

bool command_init(struct command *cmd);
bool command_load(struct command *cmd);
bool command_unload(struct command *cmd);

#endif
