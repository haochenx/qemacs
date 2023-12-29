external foo : int -> int -> int = "foo"
external bar : int -> unit = "bar"

let counter = ref 0

let go() =
  counter := !counter + 1;
  foo (!counter+7) (foo 2 4) |> bar

let () =
  Callback.register "standalone1_go" go;
  go()
