#ifndef EDITLITE_UNICODE_IMP_H
#define EDITLITE_UNICODE_IMP_H

#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct unicode_str;
struct utf8_buffer;

bool unicode_str_init(struct unicode_str *) __THROWNL __nonnull((1));
void unicode_str_free(struct unicode_str *) __THROWNL __nonnull((1));

void utf8_buffer_free(struct utf8_buffer *) __THROWNL __nonnull((1));

__END_DECLS

#endif
