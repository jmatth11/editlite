#include <SDL2/SDL_render.h>
#include <string.h>
#include "gap_buffer.h"
#include "glyph.h"
#include "menu.h"
#include "page.h"
#include "plugin_interface.h"
#include "util.h"
#include "display.h"
#include "win.h"

int display_init(struct display* d) {
  // we do not initialize the menu in here because we initialize it on the fly
  // when we enter command mode
  d->mode = NORMAL;
  d->cursor.pos = (struct display_dim){0, 0};
  d->cursor.screen_pos = d->cursor.pos;
  d->cur_buf = 0;
  d->glyphs.color = d->config.font_color;
  d->glyphs.point = d->config.font_point;
  d->glyphs.scale = d->config.font_scale;
  d->running = true;
  d->menu.items.menu_item_data = NULL;
  d->menu.items.len = 0;
  d->menu.items.cap = 0;
  plugin_interface_init(&d->pi);
  d->pi.__internal = d;
  int err = init_char(&d->glyphs, &d->w, d->config.font_file);
  if (err != 0) return err;
  err = page_manager_init(&d->page_mgr);
  if (err != 0) return err;
  err = init_command_array(&d->cmds, 1);
  if (err != 0) return err;
  if (!display_load_plugins(d)) return 1;
  return 0;
}

bool display_load_plugins(struct display* d) {
  for (int i = 0; i < d->config.plugins.len; ++i) {
    struct command cmd;
    command_init(&cmd);
    cmd.shared_library = d->config.plugins.string_data[i];
    insert_command_array(&d->cmds, cmd);
  }
  for (int i = 0; i < d->cmds.len; ++i) {
    struct command *cmd = &d->cmds.command_data[i];
    if (!command_load(cmd)) {
      fprintf(stderr, "failed command load.\n");
    };
    if (cmd->setup != NULL) {
      if (!cmd->setup(&d->pi)) {
        const char *prompt;
        cmd->get_display_prompt(&prompt);
        fprintf(stderr, "plugin setup failed: \"%s\"\n", prompt);
      }
    }
  }
  return true;
}

bool display_reload_plugins(struct display *d) {
  for (int i = 0; i < d->cmds.len; ++i) {
    struct command *cmd = &d->cmds.command_data[i];
    if (!command_unload(cmd)) {
      fprintf(stderr, "failed command load.\n");
    };
  }
  for (int i = 0; i < d->cmds.len; ++i) {
    struct command *cmd = &d->cmds.command_data[i];
    if (!command_load(cmd)) {
      fprintf(stderr, "failed command load.\n");
    };
  }
  return true;
}

SDL_Texture * handle_characters(struct display *d, const char cur_char) {
  if (cur_char == '\t') {
    return get_glyph(&d->glyphs, ' ');
  }
  return get_glyph(&d->glyphs, cur_char);
}

void reset_cursor_screen_pos(struct display *d) {
  d->cursor.screen_pos = d->cursor.pos;
}

void draw_cursor(struct display *d, SDL_Rect *rect) {
  const SDL_Color c = d->config.cursor_color;
  SDL_SetRenderDrawColor(d->w.renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(d->w.renderer, rect);
}

bool display_page_render(struct display *d) {
  if (d->page_mgr.pages.len < 1) {
    return false;
  }
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page.\n");
    return false;
  }
  struct display_dim dims;
  display_get_page_dim(d, &dims);
  int width_offset=cur_page->x_offset;
  int height_offset=cur_page->y_offset;
  reset_cursor_screen_pos(d);
  const size_t font_width = d->glyphs.width;
  const size_t font_height = d->glyphs.height;
  const int line_start = cur_page->row_offset;
  const int line_end = dims.row + cur_page->row_offset;
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, line_start);
  for (int line_idx = line_start; line_idx < line_end; ++line_idx) {
    if (cur_line == NULL) break;
    const int char_start = cur_page->col_offset;
    const int gap_buffer_len = gap_buffer_get_len(&cur_line->value.chars);
    const int char_len = MIN(gap_buffer_len, dims.col + cur_page->col_offset);
    if (d->cursor.pos.row == line_idx && d->cursor.pos.col > char_len) {
      d->cursor.screen_pos.col = char_len;
    }
    SDL_Rect r;
    for (int char_idx = char_start; char_idx < char_len; ++char_idx) {
      char cur_char = ' ';
      gap_buffer_get_char(&cur_line->value.chars, char_idx, &cur_char);
      SDL_Texture *glyph = handle_characters(d, cur_char);
      r = (SDL_Rect){
        .x = width_offset,
        .y = height_offset,
        .w = font_width,
        .h = font_height,
      };
      if (glyph == NULL) {
        // ignore newline character and carriage return
        if (cur_char != 10 && cur_char != 13) {
          fprintf(stderr, "cur_char: %d\n", cur_char);
          fprintf(stderr, "glyph was null\n");
          // display question mark for unknown chars
          glyph = handle_characters(d, '?');
        } else if (d->cursor.screen_pos.row == line_idx &&
          d->cursor.screen_pos.col == char_idx ) {
          draw_cursor(d, &r);
          continue;
        }
      }
      if (d->cursor.screen_pos.row == line_idx && d->cursor.screen_pos.col == char_idx) {
        draw_cursor(d, &r);
      }
      SDL_RenderCopy(
        d->w.renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += font_width;
    }
    // this is for empty lines
    if (d->cursor.screen_pos.row == line_idx && d->cursor.screen_pos.col == char_len) {
      draw_cursor(d, &r);
    }
    width_offset = 0;
    height_offset += font_height;
    cur_line = cur_line->next;
  }
  return true;
}

bool display_get_cur_page(struct display *d, struct page **out) {
  // Typical flow.
  if (d->page_mgr.pages.len == 0) {
    struct page cur_page;
    if (!page_init(&cur_page)) {
      fprintf(stderr, "could not initialize page.\n");
      return false;
    }
    insert_page_array(&d->page_mgr.pages, cur_page);
  }
  if (d->cur_buf >= 0 && d->cur_buf < d->page_mgr.pages.len) {
    struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
    if (cur_page->fp == NULL && cur_page->file_name != NULL) {
      if (!d->page_mgr.open(cur_page)) {
        fprintf(stderr, "failed to open file \"%s\".\n", cur_page->file_name);
        return false;
      }
      if (!d->page_mgr.read(cur_page)) {
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

void display_get_page_dim(struct display *d, struct display_dim *out) {
  int winh, winw;
  SDL_GetWindowSize(d->w.window, &winw, &winh);
  out->row = (int)winh / d->glyphs.height;
  out->col = ((int)winw / d->glyphs.width);
}

void display_free(struct display* d) {
  for (int i = 0; i < d->cmds.len; ++i) {
    struct command *cmd = &d->cmds.command_data[i];
    if (cmd->cleanup != NULL) {
      if (!cmd->cleanup(&d->pi)) {
        const char *prompt;
        cmd->get_display_prompt(&prompt);
        fprintf(stderr, "clean up for command failed: \"%s\"\n", prompt);
      }
    }
  }
  free_command_array(&d->cmds);
  free_char(&d->glyphs);
  page_manager_free(&d->page_mgr);
  free_command_array(&d->cmds);
  win_free(&d->w);
}
