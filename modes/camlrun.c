#include <caml/mlvalues.h>
#include <caml/callback.h>

#include "qe.h"

extern void caml_startup(char_os ** argv);
extern void caml_shutdown(void);

extern CAMLprim value foo(value x, value y);
extern CAMLprim value bar(value x);

static void call_caml_callbackU(char* name){
  const value *f = caml_named_value(name);
  if (f != NULL)
    caml_callback_exn(*f, Val_unit);
}

void do_camlrun_test(EditState *s) {
  /* bar(Val_int(13)); */
  call_caml_callbackU("standalone1_go");
}


static const CmdDef camlrun_commands[] = {
  CMD0("camlrun_test", "M-C-g c", "camlrun test",
       do_camlrun_test)
};

static int camlrun_init(void) {
  char_os *argv[] = {
    "qe.camlrun",
    0,
  };
  caml_startup(argv); // we probably want to do this lazily

  qe_register_commands(NULL, camlrun_commands, countof(camlrun_commands));
  return 0;
}

qe_module_init(camlrun_init);
