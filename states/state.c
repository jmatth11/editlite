#include "commands/command.h"
#include "pages/page.h"
#include "state.h"
#include "structures/gap_buffer.h"
#include "structures/linked_list.h"
#include "types/command_types.h"
#include "types/page_types.h"
#include "types/size_types.h"
#include "types/state_types.h"

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
