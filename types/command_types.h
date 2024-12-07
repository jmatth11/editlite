#ifndef EDITLITE_COMMAND_TYPES_H
#define EDITLITE_COMMAND_TYPES_H

#include <stdbool.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include "../deps/array_template/array_template.h"

// forward declares
struct display;
struct display_dim;
struct plugin_interface;

struct command {
  void* handler;
  char *shared_library;
  /**
   * Optional
   * Entry point for setting up your plugin.
   * This is called at launch.
   */
  bool (*setup)(struct plugin_interface *);
  /**
   * !! Mandatory !!
   * Called when the user executes your plugin from the command menu.
   * This is when you can perform an action or request certain actions with dispatch calls.
   */
  bool (*action)(struct plugin_interface *);
  /**
   * !! Mandatory !!
   * Called when displaying the name of your plugin.
   */
  void (*get_display_prompt)(const char **out);
  /**
   * Optional
   * If your plugin needs to render things to the screen.
   * This gets called on every frame update.
   */
  bool (*render)(struct display *, struct display_dim *);
  /**
   * Optional
   * If your plugin needs to listen for SDL events.
   * This gets called on every frame update.
   */
  bool (*event)(SDL_Event *, struct display *, struct display_dim *);
  /**
   * Optional
   * Entry point for cleaning up your plugin.
   * This gets called when the user is exiting the app.
   */
  bool (*cleanup)(struct plugin_interface *);
};

generate_array_template(command, struct command);

#endif
