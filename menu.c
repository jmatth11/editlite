#include "menu.h"
#include "display.h"
#include "win.h"

bool menu_init(struct menu *m) {
  m->w = 50;
  m->h = 60;
  m->x = 0;
  m->y = 0;
  m->idx = 0;
  return true;
}

void draw_select(struct display *d, struct win *w, SDL_Rect *rect) {
  const SDL_Color c = d->config.cursor_color;
  SDL_SetRenderDrawColor(w->renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(w->renderer, rect);
}

bool menu_display(struct display *d, struct win *w) {
  int win_h, win_w;
  SDL_GetWindowSize(w->window, &win_w, &win_h);
  const int width = win_w - 50;
  const int height = 60;
  const int x_offset = (win_w / 2) - (width / 2);
  const int y_offset = (win_h /2) - height;
  SDL_Rect r = {
    .x = x_offset,
    .y = y_offset,
    .h = height,
    .w = width,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  size_t cmd_len = d->cmds.len;
  int width_offset = x_offset;
  int height_offset = y_offset;
  for (size_t i = 0; i < cmd_len; ++i) {
    r = (SDL_Rect){
      .x = width_offset,
      .y = height_offset,
      .w = width,
      .h = d->glyphs.max_height,
    };
    if (d->menu.idx == i) {
      draw_select(d, w, &r);
    }
    struct command *cmd = &d->cmds.command_data[i];
    const char *prompt;
    cmd->get_display_prompt(&prompt);
    for (size_t char_idx = 0; char_idx < strlen(prompt); ++char_idx) {
      SDL_Texture *glyph = handle_characters(d, prompt[char_idx]);
      if (glyph == NULL) {
        glyph = handle_characters(d, '?');
      }
      r = (SDL_Rect){
        .x = width_offset,
        .y = height_offset,
        .w = d->glyphs.max_width,
        .h = d->glyphs.max_height,
      };
      SDL_RenderCopy(
        w->renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += d->glyphs.max_width;
    }
    height_offset += d->glyphs.max_height;
    width_offset = x_offset;
  }
  return true;
}
