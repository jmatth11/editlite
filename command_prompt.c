#include "command_prompt.h"
#include <stdlib.h>
#include <dlfcn.h>

bool command_init(struct command *cmd) {
  cmd->handler = NULL;
  cmd->shared_library = NULL;
  cmd->action = NULL;
  cmd->render = NULL;
  cmd->event = NULL;
  cmd->get_display_prompt = NULL;
  cmd->setup = NULL;
  cmd->cleanup = NULL;
  return true;
}

bool command_load(struct command *cmd) {
  if (cmd->shared_library == NULL) {
    fprintf(stderr, "shared library file path not set.\n");
    return false;
  }
  if (cmd->handler != NULL) {
    return true;
  }
  void *handle = dlopen(cmd->shared_library, RTLD_LAZY);
  if (handle == NULL) {
    fprintf(stderr, "could not load shared library: \"%s\"\n", cmd->shared_library);
    return false;
  }
  // these functions aren't necessary
  cmd->setup = dlsym(handle, "setup");
  cmd->cleanup = dlsym(handle, "cleanup");
  // these functions are required
  cmd->action = dlsym(handle, "action");
  if (cmd->action == NULL) {
    fprintf(stderr,
            "action function was not defined in shared library: \"%s\"\n",
            cmd->shared_library);
    return false;
  }
  cmd->get_display_prompt = dlsym(handle, "get_display_prompt");
  if (cmd->get_display_prompt == NULL) {
    fprintf(stderr,
            "get_display_prompt function was not defined in shared library: \"%s\"\n",
            cmd->shared_library);
    return false;
  }
  cmd->handler = handle;
  return true;
}

bool command_unload(struct command *cmd) {
  if (cmd->handler != NULL) {
    dlclose(cmd->handler);
  }
  command_init(cmd);
  return true;
}
