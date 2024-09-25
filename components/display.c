#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <stdbool.h>
#include <string.h>

#include "commands/command.h"
#include "helpers/config.h"
#include "display.h"
#include "states/registry.h"
#include "structures/gap_buffer.h"
#include "glyph.h"
#include "structures/linked_list.h"
#include "menu.h"
#include "pages/page.h"
#include "commands/plugin_interface.h"
#include "states/state.h"
#include "types/cursor_types.h"
#include "types/display_type.h"
#include "types/registered_functions.h"
#include "types/size_types.h"
#include "helpers/util.h"
#include "win.h"

struct draw_info {
  struct display *d;
  struct page *cur_page;
  struct linked_list *cur_line;
  struct cursor *cursor;
  struct display_dim dims;
  size_t line_idx;
  size_t width_offset;
  size_t height_offset;
  struct font_size font_size;
};

int display_init(struct display* d) {
  // we do not initialize the menu in here because we initialize it on the fly
  // when we enter command mode
  d->mode = NORMAL;
  d->plugin_mode_name = NULL;
  d->switching_mode = false;
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
  if (!registry_init(&d->state.registry)) {
    fprintf(stderr, "error initializing registry.\n");
    return 1;
  }
  if (!state_load_plugins(&d->state)) {
    fprintf(stderr, "loading plugins failed\n");
    return 1;
  }
  d->texture_from_char = handle_characters;
  return 0;
}

SDL_Texture * handle_characters(struct display *d, const code_point_t cur_char) {
  // TODO properly handle tabs at some point
  if (cur_char == '\t') {
    return get_glyph(&d->state.glyphs, &d->state.w, ' ');
  }
  return get_glyph(&d->state.glyphs, &d->state.w, cur_char);
}

void display_set_mode(struct display *d, enum display_mode mode) {
  if (d->mode != INSERT && mode == INSERT) {
    SDL_StartTextInput();
  } else if (d->mode == INSERT && mode != INSERT) {
    SDL_StopTextInput();
  }
  if (d->mode != PLUGIN_INSERT && mode == PLUGIN_INSERT) {
    SDL_StartTextInput();
  } else if (d->mode == PLUGIN_INSERT && mode != PLUGIN_INSERT) {
    SDL_StopTextInput();
    d->plugin_mode_name = NULL;
  }
  d->mode = mode;
  d->switching_mode = true;
}

static void inline draw_cursor(struct display *d, SDL_Rect *rect) {
  const SDL_Color c = d->state.config.cursor_color;
  SDL_SetRenderDrawColor(d->state.w.renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(d->state.w.renderer, rect);
}

static struct character_display inline generate_character_display(struct draw_info di, size_t char_idx) {
  struct character_display cd;
  code_point_t cur_char = ' ';
  gap_buffer_get_char(&di.cur_line->value.chars, char_idx, &cur_char);
  cd.buf = cur_char;
  cd.glyph = handle_characters(di.d, cur_char);
  cd.display_pos = (SDL_Rect){
    .x = di.width_offset,
    .y = di.height_offset,
    .w = di.font_size.width,
    .h = di.font_size.height,
  };
  cd.row = di.line_idx;
  cd.col = char_idx;
  return cd;
}

static void inline draw_line(struct draw_info d) {
  const int char_start = d.cur_page->page_offset.col;
  const int gap_buffer_len = gap_buffer_get_len(&d.cur_line->value.chars);
  const int char_len = MIN(gap_buffer_len, d.dims.col + d.cur_page->page_offset.col);
  const render_glyph_func_array plugin_render_glyph = d.d->state.registry.render_glyph_funcs;
  if (d.cursor->pos.row == d.line_idx && d.cursor->pos.col > char_len) {
    d.cursor->screen_pos.col = char_len;
  }
  for (int char_idx = char_start; char_idx < char_len; ++char_idx) {
    bool mutated = false;
    struct character_display cd = generate_character_display(d, char_idx);
    if (cd.glyph == NULL) {
      // ignore newline character and carriage return
      if (cd.buf != 10 && cd.buf != 13) {
        fprintf(stderr, "cur_char: %d\n", cd.buf);
        fprintf(stderr, "glyph was null\n");
        // display question mark for unknown chars
        // TODO change to recommended unicode questions mark symbol
        cd.glyph = handle_characters(d.d, '?');
      } else if (d.cursor->screen_pos.row == d.line_idx &&
        d.cursor->screen_pos.col == char_idx ) {
        draw_cursor(d.d, &cd.display_pos);
        continue;
      }
    }
    if (d.cursor->screen_pos.row == d.line_idx && d.cursor->screen_pos.col == char_idx) {
      draw_cursor(d.d, &cd.display_pos);
    }
    if (plugin_render_glyph.len > 0) {
      for (int render_idx = 0; render_idx < plugin_render_glyph.len; ++render_idx) {
        render_glyph render_func = plugin_render_glyph.render_glyph_func_data[render_idx];
        if (render_func != NULL && cd.glyph != NULL) {
          mutated = true;
          render_func(&cd);
        }
      }
    }
    SDL_RenderCopy(
      d.d->state.w.renderer,
      cd.glyph,
      NULL,
      &cd.display_pos
    );
    // if the character was possibly mutated, change it back.
    // TODO maybe need to revisit because there might be more attributes we will need to reset.
    if (mutated) {
      const SDL_Color orig_color = d.d->state.glyphs.color;
      SDL_SetTextureColorMod(cd.glyph, orig_color.r, orig_color.g, orig_color.b);
    }
    d.width_offset += d.font_size.width;
  }
  // this is for empty lines
  SDL_Rect r = {
    .x = d.width_offset - d.font_size.width,
    .y = d.height_offset,
    .w = d.font_size.width,
    .h = d.font_size.height
  };
  if (d.cursor->screen_pos.row == d.line_idx && d.cursor->screen_pos.col == char_len) {
    draw_cursor(d.d, &r);
  }
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
    struct draw_info di = {
      .d = d,
      .cur_page = cur_page,
      .cur_line = cur_line,
      .cursor = cursor,
      .dims = dims,
      .line_idx = line_idx,
      .width_offset = width_offset,
      .height_offset = height_offset,
      .font_size = font_size,
    };
    draw_line(di);
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
  registry_free(&d->state.registry);
  free_char(&d->state.glyphs);
  page_manager_free(&d->state.page_mgr);
  free_command_array(&d->state.cmds);
  win_free(&d->state.w);
}
