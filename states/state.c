#include "commands/command.h"
#include "commands/plugin_interface.h"
#include "pages/page.h"
#include "registry.h"
#include "scribe.h"
#include "state.h"
#include "states/sessions.h"
#include "structures/gap_buffer.h"
#include "structures/linked_list.h"
#include "types/command_types.h"
#include "types/page_types.h"
#include "types/session_types.h"
#include "types/size_types.h"
#include "types/state_types.h"
#include "components/glyph.h"
#include "helpers/config.h"
#include "components/win.h"
#include <SDL2/SDL_keycode.h>
#include <stddef.h>
#include <stdint.h>

static int state_writer_at(void* ptr, struct Edit e) {
  struct app_state* state = (struct app_state*)ptr;
  struct page *cur_page;
  if (!state_get_cur_page(state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
    return 0;
  }
  if (e.character == SDLK_RETURN) {
    if (!cur_page->handle_newline(cur_page, e.row, e.col)) {
      fprintf(stderr, "error with writer_at for newline\n");
      return 0;
    }
  } else {
    if (!cur_page->handle_keystroke(cur_page, e.character, e.row, e.col)) {
      fprintf(stderr, "error with writer_at for keystroke\n");
      return 0;
    }
  }
  return 1;
}

static int state_delete_at(void *ptr, struct Edit e) {
  struct app_state* state = (struct app_state*)ptr;
  struct page *cur_page;
  if (!state_get_cur_page(state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
    return 0;
  }
  if (!cur_page->handle_backspace(cur_page, e.row, e.col)) {
    fprintf(stderr, "error with deleter_at\n");
    return 0;
  }
  return 1;
}

bool state_init(struct app_state* state) {
  state->cur_buf = 0;
  state->glyphs.color = state->config.font_color;
  state->glyphs.point = state->config.font_point;
  state->glyphs.scale = state->config.font_scale;
  state->running = true;
  state->menu.items.menu_item_data = NULL;
  state->menu.items.len = 0;
  state->menu.items.cap = 0;
  plugin_interface_init(&state->pi);
  int err = init_char(&state->glyphs, &state->w, state->config.font_file);
  if (err != 0) return false;
  if (!page_manager_init(&state->page_mgr)) {
    fprintf(stderr, "page manager failed to initialize\n");
    return false;
  }
  if(!init_command_array(&state->cmds, 1)) {
    fprintf(stderr, "command array init failed\n");
    return false;
  }
  if (!registry_init(&state->registry)) {
    fprintf(stderr, "error initializing registry.\n");
    return false;
  }
  if (!state_load_plugins(state)) {
    fprintf(stderr, "loading plugins failed\n");
    return false;
  }
  if (!init_session_array(&state->sessions, 1)) {
    fprintf(stderr, "sessions array init failed\n");
    return false;
  }
  struct session og = {
    .page_idx = 0,
  };
  if (!session_add(&state->sessions, og)) {
    fprintf(stderr, "adding session failed\n");
    return false;
  }
  struct ScribeWriter writer = {
    .ptr = state,
    .write_at = state_writer_at,
    .delete_at = state_delete_at,
  };
  if (!scribe_init(&state->scribe, writer)) {
    fprintf(stderr, "initializing scribe failed\n");
    return false;
  }
  return true;
}

void state_free(struct app_state* state) {
  for (int i = 0; i < state->cmds.len; ++i) {
    struct command *cmd = &state->cmds.command_data[i];
    if (cmd->cleanup != NULL) {
      if (!cmd->cleanup(&state->pi)) {
        const char *prompt;
        cmd->get_display_prompt(&prompt);
        fprintf(stderr, "clean up for command failed: \"%s\"\n", prompt);
      }
    }
    free(cmd->shared_library);
  }
  config_free(&state->config);
  free_command_array(&state->cmds);
  registry_free(&state->registry);
  free_char(&state->glyphs);
  page_manager_free(&state->page_mgr);
  free_command_array(&state->cmds);
  win_free(&state->w);
}

bool state_load_plugins(struct app_state *state) {
  if (state->cmds.command_data == NULL) {
    if (!init_command_array(&state->cmds, state->config.plugins.len)) {
      return false;
    }
  }
  size_t plugin_path_len = strlen(state->config.plugin_path);
  for (int i = 0; i < state->config.plugins.len; ++i) {
    struct command cmd;
    command_init(&cmd);
    const char *plugin_str = state->config.plugins.string_data[i];
    size_t plugin_len = strlen(plugin_str);
    cmd.shared_library = malloc(sizeof(char) * (plugin_path_len + plugin_len + 2));
    sprintf(cmd.shared_library, "%s%c%s", state->config.plugin_path, '/', plugin_str);
    cmd.shared_library[plugin_path_len + plugin_len + 1] = '\0';
    if (command_load(&cmd)) {
      if (cmd.setup != NULL) {
        if (!cmd.setup(&state->pi)) {
          const char *prompt;
          cmd.get_display_prompt(&prompt);
          fprintf(stderr, "plugin setup failed: \"%s\"\n", prompt);
        }
      }
      insert_command_array(&state->cmds, cmd);
    } else {
      fprintf(stderr, "failed command load: \"%s\"\n", cmd.shared_library);
    }
  }
  return true;
}

bool state_reload_plugins(struct app_state *state) {
  for (int i = 0; i < state->cmds.len; ++i) {
    struct command *cmd = &state->cmds.command_data[i];
    if (!command_unload(cmd)) {
      fprintf(stderr, "failed command load.\n");
    };
  }
  for (int i = 0; i < state->cmds.len; ++i) {
    struct command *cmd = &state->cmds.command_data[i];
    if (!command_load(cmd)) {
      fprintf(stderr, "failed command load.\n");
    };
  }
  return true;
}

bool state_get_cur_page(struct app_state *state, struct page **out) {
  if (state->page_mgr.pages.len == 0) {
    struct page cur_page;
    if (!page_init(&cur_page)) {
      fprintf(stderr, "could not initialize page.\n");
      return false;
    }
    insert_page_array(&state->page_mgr.pages, cur_page);
  }
  if (state->cur_buf >= 0 && state->cur_buf < state->page_mgr.pages.len) {
    struct page *cur_page = &state->page_mgr.pages.page_data[state->cur_buf];
    if (cur_page->fp == NULL && cur_page->file_name != NULL) {
      if (!state->page_mgr.open(cur_page)) {
        fprintf(stderr, "failed to open file \"%s\".\n", cur_page->file_name);
        return false;
      }
      if (!state->page_mgr.read(cur_page, state->config.read_size)) {
        fprintf(stderr, "failed to read file \"%s\".\n", cur_page->file_name);
        return false;
      }
    }
    if (cur_page->fp == NULL) {
      struct gap_buffer *gb = &cur_page->lines->value.chars;
      // ensure there is at least a newline in the gap buffer
      if (gap_buffer_get_len(gb) == 0) {
        gap_buffer_insert(gb, '\n');
        gap_buffer_move_cursor(gb, 0);
      }
    }
    *out = cur_page;
    return true;
  }
  return false;
}

void state_get_page_dim(struct app_state *state, struct display_dim *out) {
  int winh, winw;
  SDL_GetWindowSize(state->w.window, &winw, &winh);
  out->row = (int)winh / state->glyphs.scaled_size.height;
  out->col = ((int)winw / state->glyphs.scaled_size.width);
}
