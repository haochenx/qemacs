#define MAX_PASTE_SIZE 1024 * 1024 * 12 /* 12M; should be a good rule of thumb */
#include "qe.h"

#ifdef CONFIG_DARWIN

#include "darwin/clipboard.h"

void do_clipboard_paste(EditState *s) {
  size_t incoming_size;
  int size;
  QEmacsState *qs = s->qe_state;
  char *b;

  b = pbpaste(&incoming_size, (char* (*)(size_t))qe_malloc_bytes);

  if (incoming_size > MAX_PASTE_SIZE) size = MAX_PASTE_SIZE;
  else size = (int) incoming_size;

  // mirroring do_yank
  selection_request(qs->screen);

  if (size > 0) {
    s->b->last_log = LOGOP_FREE;
    s->offset += eb_insert(s->b, s->offset, b, size);
  }
  qs->this_cmd_func = (CmdFunc)do_clipboard_paste;

  if (incoming_size != (size_t) size) {
    put_status(s, "Pasted %d bytes (clipped off %zu bytes) from the system clipboard.",
               size, incoming_size);
  } else {
    put_status(s, "Pasted %d bytes from the system clipboard.", size);
  }
}

void do_clipboard_copy(EditState *s) {
  QEmacsState *qs = s->qe_state;
  int len, tmp;
  EditBuffer *b;
  int p1, p2, dir, keep;
  char *buf;
  int successful;

  /* from do_copy_region */
  p1 = s->b->mark;
  p2 = s->offset;
  dir = 0;
  keep = 1;

  // borrowed from do_kill

  /* deactivate region hilite */
  s->region_style = 0;

  if (p1 > p2) {
    tmp = p1;
    p1 = p2;
    p2 = tmp;
  }
  len = p2 - p1;

  buf = qe_malloc_bytes(len);
  eb_read(s->b, p1, buf, len);

  successful = pbcopy2(buf, len);

  selection_activate(qs->screen);
  qs->this_cmd_func = (CmdFunc)do_clipboard_copy;

  if (successful) {
    put_status(s, "%d bytes copied into the system clipboard.", len);
  } else {
    put_status(s, "Failed to copy %d bytes into the system clipboard.", len);
  }
}

static const CmdDef clipboard_commands[] = {
  CMD0("clipboard-paste-from", "C-u C-y", "paste from the system clipboard",
       do_clipboard_paste)
  CMD0("clipboard-copy-into", "C-u M-w", "copy into the system clipboard",
       do_clipboard_copy)
};

static int clipboard_darwin_init(void) {
  qe_register_commands(NULL, clipboard_commands, countof(clipboard_commands));
  return 0;
}

qe_module_init(clipboard_darwin_init);

#endif /* CONFIG_DARWIN */

