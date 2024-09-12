#include <string.h>

#include "commands/command.h"
#include "helpers/config.h"
#include "display.h"
#include "structures/gap_buffer.h"
#include "glyph.h"
#include "structures/linked_list.h"
#include "menu.h"
#include "pages/page.h"
#include "commands/plugin_interface.h"
#include "states/state.h"
#include "types/cursor_types.h"
#include "types/display_type.h"
#include "types/size_types.h"
#include "helpers/util.h"
#include "win.h"


int display_init(struct display* d) {
  // we do not initialize the menu in here because we initialize it on the fly
  // when we enter command mode
  d->mode = NORMAL;
  d->state.cur_buf = 0;
  d->state.glyphs.color = d->state.config.font_color;
  d->state.glyphs.point = d->state.config.font_point;
  d->state.glyphs.scale = d->state.config.font_scale;
  d->state.running = true;
  d->state.menu.items.menu_item_data = NULL;
  d->state.menu.items.len = 0;
  d->state.menu.items.cap = 0;
  plugin_interface_init(&d->state.pi);
  d->state.pi.__internal = d;
  int err = init_char(&d->state.glyphs, &d->state.w, d->state.config.font_file);
  if (err != 0) return err;
  if (!page_manager_init(&d->state.page_mgr)) {
    fprintf(stderr, "page manager failed to initialize\n");
    return 1;
  }
  if(!init_command_array(&d->state.cmds, 1)) {
    fprintf(stderr, "command array init failed\n");
    return 1;
  }
  if (!state_load_plugins(&d->state)) {
    fprintf(stderr, "loading plugins failed\n");
    return 1;
  }
  d->texture_from_char = handle_characters;
  return 0;
}

SDL_Texture * handle_characters(struct display *d, const char cur_char) {
  // TODO properly handle tabs at some point
  if (cur_char == '\t') {
    return get_glyph(&d->state.glyphs, ' ');
  }
  return get_glyph(&d->state.glyphs, cur_char);
}

static void inline draw_cursor(struct display *d, SDL_Rect *rect) {
  const SDL_Color c = d->state.config.cursor_color;
  SDL_SetRenderDrawColor(d->state.w.renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(d->state.w.renderer, rect);
}

bool display_page_render(struct display *d) {
  if (d->state.page_mgr.pages.len < 1) {
    fprintf(stderr, "page mgr has no pages.\n");
    return false;
  }
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page.\n");
    return false;
  }
  struct display_dim dims;
  state_get_page_dim(&d->state, &dims);
  int width_offset=cur_page->position_offset.x;
  int height_offset=cur_page->position_offset.y;
  page_reset_screen_cursor(cur_page);
  const struct font_size font_size = d->state.glyphs.scaled_size;
  const int line_start = cur_page->page_offset.row;
  const int line_end = dims.row + line_start;
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, line_start);
  struct cursor *cursor = &cur_page->cursor;
  for (int line_idx = line_start; line_idx < line_end; ++line_idx) {
    if (cur_line == NULL) break;
    const int char_start = cur_page->page_offset.col;
    const int gap_buffer_len = gap_buffer_get_len(&cur_line->value.chars);
    const int char_len = MIN(gap_buffer_len, dims.col + cur_page->page_offset.col);
    if (cursor->pos.row == line_idx && cursor->pos.col > char_len) {
      cursor->screen_pos.col = char_len;
    }
    SDL_Rect r;
    for (int char_idx = char_start; char_idx < char_len; ++char_idx) {
      char cur_char = ' ';
      gap_buffer_get_char(&cur_line->value.chars, char_idx, &cur_char);
      SDL_Texture *glyph = handle_characters(d, cur_char);
      r = (SDL_Rect){
        .x = width_offset,
        .y = height_offset,
        .w = font_size.width,
        .h = font_size.height,
      };
      if (glyph == NULL) {
        // ignore newline character and carriage return
        if (cur_char != 10 && cur_char != 13) {
          fprintf(stderr, "cur_char: %d\n", cur_char);
          fprintf(stderr, "glyph was null\n");
          // display question mark for unknown chars
          glyph = handle_characters(d, '?');
        } else if (cursor->screen_pos.row == line_idx &&
          cursor->screen_pos.col == char_idx ) {
          draw_cursor(d, &r);
          continue;
        }
      }
      if (cursor->screen_pos.row == line_idx && cursor->screen_pos.col == char_idx) {
        draw_cursor(d, &r);
      }
      SDL_RenderCopy(
        d->state.w.renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += font_size.width;
    }
    // this is for empty lines
    if (cursor->screen_pos.row == line_idx && cursor->screen_pos.col == char_len) {
      draw_cursor(d, &r);
    }
    width_offset = cur_page->position_offset.x;
    height_offset += font_size.height;
    cur_line = cur_line->next;
  }
  return true;
}

void display_free(struct display* d) {
  for (int i = 0; i < d->state.cmds.len; ++i) {
    struct command *cmd = &d->state.cmds.command_data[i];
    if (cmd->cleanup != NULL) {
      if (!cmd->cleanup(&d->state.pi)) {
        const char *prompt;
        cmd->get_display_prompt(&prompt);
        fprintf(stderr, "clean up for command failed: \"%s\"\n", prompt);
      }
    }
    free(cmd->shared_library);
  }
  config_free(&d->state.config);
  free_command_array(&d->state.cmds);
  free_char(&d->state.glyphs);
  page_manager_free(&d->state.page_mgr);
  free_command_array(&d->state.cmds);
  win_free(&d->state.w);
}
