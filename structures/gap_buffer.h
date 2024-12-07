#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Simple Gap Buffer structure for handling insertion and deletion of text.
 */
struct gap_buffer {
  size_t cap;
  size_t len;
  size_t cursor_start;
  size_t cursor_end;
  char* (*get_str)(struct gap_buffer*);
  char *buffer;
};

/**
 * Initialize the gap buffer with the given size.
 * @param gb The gap buffer.
 * @param buf_size The size of the initial buffer.
 * @returns True if successful, false otherwise.
 */
bool gap_buffer_init(struct gap_buffer *gb, size_t buf_size);

/**
 * Move the cursor of the buffer to the given position.
 * @param gb The gap buffer.
 * @param pos The position offset to move to.
 * @returns True if successful, false otherwise.
 */
bool gap_buffer_move_cursor(struct gap_buffer *gb, size_t pos);

/**
 * Get the char at the given position.
 * @param gb The gap buffer.
 * @param pos The position offset.
 * @param[out] out The char variable to be populated.
 */
void gap_buffer_get_char(const struct gap_buffer *gb, size_t pos, char* out);

/**
 * Get the length of the gap buffer.
 * This does not include the gap length only the text length.
 * @param gb The gap buffer.
 * @returns The length of the gap buffer.
 */
size_t gap_buffer_get_len(const struct gap_buffer *gb);

/**
 * Insert a character at the current position.
 * @param gb The gap buffer.
 * @param c The given character.
 * @returns True if successful, false otherwise.
 */
bool gap_buffer_insert(struct gap_buffer *gb, char c);
/**
 * Insert a word at the given position.
 * @param gb The gap buffer.
 * @param pos The position offset.
 * @param input The given input string.
 * @returns True if successful, false otherwise.
 */
bool gap_buffer_insert_word(struct gap_buffer *gb, size_t pos, char* input);
/**
 * Delete a character to the left of the current position.
 * @param gb The gap buffer.
 * @returns True if successful, false otherwise.
 */
bool gap_buffer_delete(struct gap_buffer *gb);
/**
 * Delete a sequence of characters to the left of the current position.
 * @param gb The gap buffer.
 * @param n The size of the sequence.
 * @returns True if successful, false otherwise.
 */
bool gap_buffer_delete_seq(struct gap_buffer *gb, size_t n);
/**
 * Get the string value of the gap buffer.
 * User is responsible for freeing the returned string.
 *
 * @param gb The gap buffer.
 * @returns The constructed string.
 */
char *gap_buffer_get_str(struct gap_buffer *gb);
/**
 * Free the gap buffer internals.
 * @param gb The gap buffer.
 */
void gap_buffer_free(struct gap_buffer *gb);

#ifdef __cplusplus
}
#endif

#endif
