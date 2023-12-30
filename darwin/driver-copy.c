// for clipboard.mm demo / testing only

#include <string.h>
#include <stdio.h>

extern int pbcopy(const char *utf8_string);

int main(void) {
  static const char *contents = "driver-copy-test";
  int successful = pbcopy(contents);
  printf("%zu bytes (%s) copied successfully? %s\n",
         strlen(contents), contents,
         successful ? "yes" : "no");
  return 0;
}
