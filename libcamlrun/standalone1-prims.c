#include "qe.h"
#include <caml/mlvalues.h>

CAMLprim value foo(value x, value y) {
  return x;
}


CAMLprim value bar(value x) {
  return Val_unit;
}

