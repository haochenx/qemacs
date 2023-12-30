#ifndef DARWIN_CLIPBOARD
#define DARWIN_CLIPBOARD

extern int pbcopy(const char *utf8_string);
extern int pbcopy2(const char *utf8_string_buf, size_t len);

extern char *pbpaste(size_t *len, char* (*allocator)(size_t));

#endif /* DARWIN_CLIPBOARD */
