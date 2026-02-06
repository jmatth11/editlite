#include <SDL2/SDL_keycode.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "components/display.h"
#include "helpers/file_ops.h"
#include "line.h"
#include "page.h"
#include "structures/gap_buffer.h"
#include "structures/linked_list.h"
#include "structures/unicode.h"
#include "types/display_type.h"
#include "types/line_types.h"
#include "types/size_types.h"
#include "types/page_types.h"
#include "types/unicode_types.h"
#include "types/registered_functions.h"
#include "utf8.h"

#ifndef BUFSIZ
#define BUFSIZ 8192
#endif
#define BUFFER_LIMIT BUFSIZ * 4

static uint8_t buffer[BUFSIZ];

bool page_handle_backspace(struct page *p, size_t row, size_t col) {
  struct linked_list *cur_line = linked_list_get_pos(p->lines, row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  if (cur_gb->cursor_start != col) {
    fprintf(stdout, "moving cursor: cursor_start:%zu, col:%zu\n", cur_gb->cursor_start, col);
    if (gap_buffer_move_cursor(cur_gb, col)) {
      fprintf(stderr, "move cursor failed in handle backspace\n");
      return false;
    }
  }
  if (col > 0) {
    gap_buffer_delete(cur_gb);
    // TODO cursor -- need to think about how to handle this per user
    // for pair programming
    p->cursor.pos.col--;
  } else if (row != 0) {
    struct linked_list *prev_line = cur_line->prev;
    struct gap_buffer *prev_gb = &prev_line->value.chars;
    gap_buffer_move_cursor(prev_gb, gap_buffer_get_len(prev_gb));
    // delete previous line's newline char
    gap_buffer_delete(prev_gb);
    size_t cur_gb_len = gap_buffer_get_len(cur_gb);
    for (int i = 0; i < cur_gb_len; ++i) {
      code_point_t tmp = ' ';
      gap_buffer_get_char(cur_gb, i, &tmp);
      gap_buffer_insert(prev_gb, tmp);
    }
    // TODO cursor -- handle this for multiple users
    p->cursor.pos.row--;
    p->cursor.pos.col = gap_buffer_get_len(prev_gb) - cur_gb_len;
    gap_buffer_move_cursor(prev_gb, p->cursor.pos.col);
    linked_list_delete_node(cur_line);
  }
  return true;
}

bool page_handle_newline(struct page *p, size_t row, size_t col) {
  struct linked_list *cur_line = linked_list_get_pos(p->lines, row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  if (cur_gb->cursor_start != col) {
    if (gap_buffer_move_cursor(cur_gb, col)) {
      fprintf(stderr, "move cursor failed in handle backspace\n");
      return false;
    }
  }
  gap_buffer_insert(cur_gb, '\n');
  size_t cur_col = col + 1;
  struct line new_line;
  init_line(&new_line);
  struct gap_buffer *next_gb = &new_line.chars;
  gap_buffer_move_cursor(next_gb, 0);
  size_t gb_len = gap_buffer_get_len(cur_gb);
  for (; cur_col < gb_len; ++cur_col) {
    code_point_t tmp = ' ';
    gap_buffer_get_char(cur_gb, cur_col, &tmp);
    gap_buffer_insert(next_gb, tmp);
  }
  cur_col = col + 1;
  gap_buffer_move_cursor(cur_gb, gb_len - 1);
  gap_buffer_delete_seq(cur_gb, gb_len - cur_col);
  // TODO cursor -- change this handling
  p->cursor.pos.col = 0;
  p->cursor.pos.row++;
  gap_buffer_move_cursor(next_gb, 0);
  linked_list_insert(cur_line, 0, new_line);
  return true;
}

void page_reset_screen_cursor(struct page *p) {
  p->cursor.screen_pos = p->cursor.pos;
}

void page_set_offset_col(struct page *p, size_t col, struct display *d) {
  p->page_offset.col = col;
  page_change_func_array events = d->state.registry.page_change_funcs;
  if (events.len > 0) {
    for (int i = 0; i < events.len; ++i) {
      page_change callback = events.page_change_func_data[i];
      if (callback != NULL) {
        callback(d, VIEW_RANGE_MOVE);
      }
    }
  }
}
void page_set_offset_row(struct page *p, size_t row, struct display *d) {
  p->page_offset.row = row;
  page_change_func_array events = d->state.registry.page_change_funcs;
  if (events.len > 0) {
    for (int i = 0; i < events.len; ++i) {
      page_change callback = events.page_change_func_data[i];
      if (callback != NULL) {
        callback(d, VIEW_RANGE_MOVE);
      }
    }
  }
}

bool page_handle_keystroke(struct page *p, code_point_t code, size_t row, size_t col) {
  if (p == NULL) {
    fprintf(stderr, "page_handle_keystroke: page is null\n");
    return false;
  }
  if (p->lines == NULL) {
    fprintf(stderr, "page_handle_keystroke: page->lines is null\n");
    return false;
  }
  struct linked_list *cur_line = linked_list_get_pos(p->lines, row);
  if (cur_line == NULL) {
    fprintf(stderr, "page_handle_keystroke: cur_line is null\n row = %ld\n", row);
    return false;
  }
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  if (cur_gb == NULL) {
    fprintf(stderr, "page_handle_keystroke: cur_gb is null\n");
    return false;
  }
  if (cur_gb->cursor_start != col) {
    if (!gap_buffer_move_cursor(cur_gb, col)) {
      fprintf(stderr, "error moving cursor\n");
      return false;
    }
  }
  if (code != '\0') {
    if (!gap_buffer_insert(cur_gb, code)) {
      fprintf(stderr, "error inserting into gap buffer for keystroke\n");
      return false;
    }
    // TODO cursor -- handle for multiple users
    p->cursor.pos.col++;
  }
  return true;
}

bool page_manager_open(struct page *buf) {
  if (buf->file_name == NULL) {
    fprintf(stderr, "file name was null.\n");
    return false;
  }
  buf->fp = fopen(buf->file_name, "r+");
  if (buf->fp != NULL) {
    fseek(buf->fp, 0L, SEEK_END);
    buf->file_size = ftell(buf->fp);
    fseek(buf->fp, 0L, SEEK_SET);
  }
  return true;
}

bool page_manager_read(struct page *buf, size_t limit) {
  bool done = buf->fp == NULL;
  int char_idx = 0;
  struct linked_list *cur_line = linked_list_get_end(buf->lines);
  struct line tmp_line;
  if (cur_line->prev == NULL) {
    tmp_line = cur_line->value;
  } else {
    init_line(&tmp_line);
    linked_list_append(cur_line, tmp_line);
    cur_line = cur_line->next;
  }
  bool create_new_line = false;
  size_t next_buffer_limit = buf->file_offset_pos + BUFFER_LIMIT;
  while (!done) {
    size_t n = fread(buffer, sizeof(uint8_t), BUFSIZ, buf->fp);
    if (n < BUFSIZ) {
      done = true;
    }
    for (int i = 0; i < n;) {
      // create new line entry on the next go so we don't have empty lines
      // at the end of the linked list
      if (create_new_line) {
        init_line(&tmp_line);
        tmp_line.start_pos = (char_idx + 1) + buf->file_offset_pos;
        tmp_line.load_pos = tmp_line.start_pos;
        linked_list_append(cur_line, tmp_line);
        cur_line = cur_line->next;
        char_idx = 0;
        create_new_line = false;
      }
      // TODO refactor this to be cleaner
      struct code_point point = utf8_next(buffer, n, i);
      if (point.type == OCT_INVALID) {
        const enum octet_type local_type = get_oct_type(buffer[i]);
        if (local_type != OCT_INVALID && local_type != OCT_NEXT) {
          const uint8_t byte_count = octet_type_count(local_type);
          uint8_t overflow[4];
          uint8_t overflow_idx = 0;
          size_t buf_idx = i;
          bool valid = true;
          while (overflow_idx < byte_count && valid) {
            if (buf_idx >= n) {
              const size_t read_len = byte_count - overflow_idx;
              const size_t read_len_n = fread(buffer, sizeof(uint8_t), read_len, buf->fp);
              if (read_len_n != read_len) {
                valid = false;
                break;
              }
              n += read_len_n;
              buf_idx = 0;
            }
            const uint8_t tmp = buffer[buf_idx];
            const enum octet_type tmp_type = get_oct_type(tmp);
            if (overflow_idx != 0 && tmp_type != OCT_NEXT) {
              valid = false;
            }
            overflow[overflow_idx] = tmp;
            overflow_idx++;
            buf_idx++;
          }
          i += overflow_idx;
          if (!valid) {
            point.val = '?';
          } else {
            point = utf8_next(overflow, byte_count, 0);
          }
        } else {
          point.val = '?';
        }
      } else {
        i += octet_type_count(point.type);
      }
      gap_buffer_insert(&cur_line->value.chars, point.val);
      if (point.val == '\n') {
        cur_line->value.load_pos = char_idx + buf->file_offset_pos;
        create_new_line = true;
      } else {
        char_idx += octet_type_count(point.type);
      }
    }
    buf->file_offset_pos += n;
    if (buf->file_offset_pos >= next_buffer_limit) {
      done = true;
    }
  }
  return true;
}

bool page_manager_write(struct page* buf) {
  FILE *tmpFile = tmpfile();
  // iterate through existing edited lines.
  struct linked_list *cur_line = buf->lines;
  while (cur_line != NULL) {
    struct gap_buffer *cur_gb = &cur_line->value.chars;
    const size_t cur_gb_len = gap_buffer_get_len(cur_gb);
    for (int i = 0; i < cur_gb_len; ++i) {
      code_point_t tmp = '?';
      gap_buffer_get_char(cur_gb, i, &tmp);
      uint8_t buf[4];
      const uint8_t byte_len = utf8_write_code_point(buf, 4, 0, tmp);
      fwrite(buf, sizeof(uint8_t), byte_len, tmpFile);
    }
    cur_line = cur_line->next;
  }
  // grab the position for our new file.
  size_t fp_idx = ftell(tmpFile);
  if (buf->fp != NULL) {
    // copy the rest of the contents
    copyFile(buf->fp, tmpFile);
    // close and reopen existing file to delete all prior content.
    fclose(buf->fp);
  }
  // safety flush
  fflush(tmpFile);
  buf->fp = fopen(buf->file_name, "w+");
  // seek to beginning of tmpFile
  fseek(tmpFile, 0, SEEK_SET);
  // copy everything over
  copyFile(tmpFile, buf->fp);
  fflush(buf->fp);
  // reset new size
  buf->file_size = ftell(buf->fp);
  // seek back to the end of our updates
  fseek(buf->fp, fp_idx, SEEK_SET);
  // close and delete tmpFile
  fclose(tmpFile);
  return true;
}

bool page_init(struct page *p) {
  p->lines = (struct linked_list*)malloc(sizeof(struct linked_list));
  int err = linked_list_init(p->lines);
  if (err != 0) {
    fprintf(stderr, "linked list failed to initialize\n");
    return false;
  }
  p->file_size = 0;
  p->file_offset_pos = 0;
  p->page_offset = (struct display_dim){0,0};
  p->position_offset = (struct SDL_Point){0,0};
  p->file_name = NULL;
  p->fp = NULL;
  p->cursor.pos = (struct display_dim){0,0};
  p->cursor.screen_pos = p->cursor.pos;
  p->handle_backspace = page_handle_backspace;
  p->handle_keystroke = page_handle_keystroke;
  p->handle_newline = page_handle_newline;
  return err == 0;
}

void page_free(struct page *p) {
  linked_list_free_all(p->lines);
  if (p->file_name != NULL) {
    free(p->file_name);
    p->file_name = NULL;
  }
  if (p->fp != NULL) {
    fclose(p->fp);
    p->fp = NULL;
  }
}

bool page_manager_init(struct page_manager *pm) {
  if (!init_page_array(&pm->pages, 1)) {
    fprintf(stderr, "page array failed to initialize\n");
    return false;
  }
  pm->open = page_manager_open;
  pm->read = page_manager_read;
  pm->write = page_manager_write;
  return true;
}

void page_manager_free(struct page_manager *pm) {
  for (int i = 0; i < pm->pages.len; ++i) {
    page_free(&pm->pages.page_data[i]);
  }
  free_page_array(&pm->pages);
}


void viewable_page_info_free(struct viewable_page_info* vpi) {
  utf8_buffer_free(&vpi->buffer);
}
