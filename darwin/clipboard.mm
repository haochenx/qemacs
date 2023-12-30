#import <Foundation/Foundation.h>
#import <AppKit/NSPasteboard.h>

static NSPasteboard* generalPasteboard() {
  static NSPasteboard* ptr = NULL;
  if (!ptr) ptr = [NSPasteboard generalPasteboard];
  return ptr;
}

extern "C"
char *pbpaste(size_t *len, char* (*allocator)(size_t)) {
  const char *cstr;
  char *dest;
  NSPasteboard* pasteboard = generalPasteboard();
  NSString* contents = [pasteboard stringForType:NSPasteboardTypeString];
  cstr = [contents UTF8String];
  *len = strlen(cstr);
  dest = allocator(*len + 1);
  if (!dest) return NULL;
  memcpy(dest, cstr, *len + 1);
  [contents dealloc];
  return dest;
}

/// return 1 if successful, or 0 otherwise
extern "C"
int pbcopy(const char *utf8_string) {
  NSString *contents = [[NSString alloc] initWithUTF8String:utf8_string];
  NSPasteboard* pasteboard = generalPasteboard();
  [pasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];
  BOOL successful = [pasteboard setString:contents forType:NSPasteboardTypeString];
  [contents dealloc];
  return (int)successful;
}

/// return 1 if successful, or 0 otherwise
extern "C"
int pbcopy2(const char *utf8_string_buf, size_t len) {
  NSString *contents =
    [[NSString alloc] initWithBytes:utf8_string_buf
                      length:(NSUInteger)len
                      encoding:NSUTF8StringEncoding];
  NSPasteboard* pasteboard = generalPasteboard();
  [pasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];
  BOOL successful = [pasteboard setString:contents forType:NSPasteboardTypeString];
  [contents dealloc];
  return (int)successful;
}
