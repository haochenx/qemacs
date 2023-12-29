external foo : int -> int -> int = "foo"
external bar : int -> unit = "bar"

let counter = ref 0

let handle_interp = function
  | "ping" -> "pong"
  | input -> "ERR: unrecognized xqcaml command: " ^ input

let go() =
  counter := !counter + 1;
  foo (!counter+7) (foo 2 4) |> bar

let () =
  Callback.register "standalone1_go" go;
  Callback.register "xqcaml_interp" handle_interp;
  go()
