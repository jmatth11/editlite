#ifndef EDITLITE_UTIL_H
#define EDITLITE_UTIL_H

#include "types/unicode_types.h"
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

/**
 * Encode a given code point array into UTF-8 format.
 *
 * @param[in] arr The code point array.
 * @param[in] len The length of the array.
 * @return The UTF-8 encoded string.
 */
uint8_t* encode_to_utf8(code_point_t *arr, size_t len) __THROWNL __nonnull((1));

__END_DECLS

#endif
