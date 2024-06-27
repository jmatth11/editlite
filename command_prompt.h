#ifndef EDITLITE_COMMAND_PROMPT_H
#define EDITLITE_COMMAND_PROMPT_H

#include <stddef.h>
#include "plugin_interface.h"
#include "deps/array_template/array_template.h"

struct command {
  void* handler;
  char *shared_library;
  bool (*setup)(struct plugin_interface *);
  bool (*action)(struct plugin_interface *);
  void (*get_display_prompt)(const char **out);
  bool (*cleanup)(struct plugin_interface *);
};

generate_array_template(command, struct command);

bool command_init(struct command *cmd);
bool command_load(struct command *cmd);
bool command_unload(struct command *cmd);

#endif
