external (+) : int -> int -> int = "%addint"
external foo : int -> int -> int = "foo"
external bar : int -> unit = "bar"

let go() = foo (3+7) (foo 2 4) |> bar
