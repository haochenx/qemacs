#include "qe.h"
#include <caml/mlvalues.h>

CAMLprim value foo(value x, value y) {
  return x;
}


CAMLprim value bar(value x) {
  fprintf(stderr, "DEBUG: bar called; arg=%d (fin)\n", Int_val(x));
  return Val_unit;
}

