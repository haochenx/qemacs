// for clipboard.mm demo / testing only

#include <stdio.h>
#include <stdlib.h>

extern char *pbpaste(size_t *len, char* (*allocator)(size_t));

int main(void) {
  size_t len = -1;
  char *pasted = pbpaste(&len, (char*(*)(size_t)) malloc);
  printf("%zu bytes pasted:\n%s\n", len, pasted);
  return 0;
}
