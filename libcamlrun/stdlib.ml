(* adopted from OCaml source tree, which has the following license notice *)
(**************************************************************************)
(*                                                                        *)
(*                                 OCaml                                  *)
(*                                                                        *)
(*             Xavier Leroy, projet Cristal, INRIA Rocquencourt           *)
(*                                                                        *)
(*   Copyright 1996 Institut National de Recherche en Informatique et     *)
(*     en Automatique.                                                    *)
(*                                                                        *)
(*   All rights reserved.  This file is distributed under the terms of    *)
(*   the GNU Lesser General Public License version 2.1, with the          *)
(*   special exception on linking described in the file LICENSE.          *)
(*                                                                        *)
(**************************************************************************)

let (|>) x f = f x

external (+) : int -> int -> int = "%addint"

external string_length : string -> int = "%string_length"
external bytes_length : bytes -> int = "%bytes_length"
external bytes_create : int -> bytes = "caml_create_bytes"
external string_blit : string -> int -> bytes -> int -> int -> unit
                     = "caml_blit_string" [@@noalloc]
external bytes_blit : bytes -> int -> bytes -> int -> int -> unit
                        = "caml_blit_bytes" [@@noalloc]
external bytes_unsafe_to_string : bytes -> string = "%bytes_to_string"
let ( ^ ) s1 s2 =
  let l1 = string_length s1 and l2 = string_length s2 in
  let s = bytes_create (l1 + l2) in
  string_blit s1 0 s 0 l1;
  string_blit s2 0 s l1 l2;
  bytes_unsafe_to_string s

external not : bool -> bool = "%boolnot"
external ( && ) : bool -> bool -> bool = "%sequand"
external ( || ) : bool -> bool -> bool = "%sequor"

external ( = ) : 'a -> 'a -> bool = "%equal"
external ( <> ) : 'a -> 'a -> bool = "%notequal"
external ( < ) : 'a -> 'a -> bool = "%lessthan"
external ( > ) : 'a -> 'a -> bool = "%greaterthan"
external ( <= ) : 'a -> 'a -> bool = "%lessequal"
external ( >= ) : 'a -> 'a -> bool = "%greaterequal"
external compare : 'a -> 'a -> int = "%compare"

type 'a ref = { mutable contents : 'a }
external ref : 'a -> 'a ref = "%makemutable"
external ( ! ) : 'a ref -> 'a = "%field0"
external ( := ) : 'a ref -> 'a -> unit = "%setfield0"
external incr : int ref -> unit = "%incr"
external decr : int ref -> unit = "%decr"
external raise : exn -> 'a = "%raise"

let invalid_arg s = raise(Invalid_argument s)

module Obj = struct
  type t

type raw_data = nativeint

external repr : 'a -> t = "%identity"
external obj : t -> 'a = "%identity"
external magic : 'a -> 'b = "%identity"
external is_int : t -> bool = "%obj_is_int"
let [@inline always] is_block a = not (is_int a)
external tag : t -> int = "caml_obj_tag" [@@noalloc]
external size : t -> int = "%obj_size"
external reachable_words : t -> int = "caml_obj_reachable_words"
external field : t -> int -> t = "%obj_field"
external set_field : t -> int -> t -> unit = "%obj_set_field"
external floatarray_get : floatarray -> int -> float = "caml_floatarray_get"
external floatarray_set :
    floatarray -> int -> float -> unit = "caml_floatarray_set"
let [@inline always] double_field x i = floatarray_get (obj x : floatarray) i
let [@inline always] set_double_field x i v =
  floatarray_set (obj x : floatarray) i v
external raw_field : t -> int -> raw_data = "caml_obj_raw_field"
external set_raw_field : t -> int -> raw_data -> unit
                                          = "caml_obj_set_raw_field"

external new_block : int -> int -> t = "caml_obj_block"
external dup : t -> t = "caml_obj_dup"
(* external add_offset : t -> Int32.t -> t = "caml_obj_add_offset" *)
external with_tag : int -> t -> t = "caml_obj_with_tag"

let first_non_constant_constructor_tag = 0
let last_non_constant_constructor_tag = 243

let forcing_tag = 244
let cont_tag = 245
let lazy_tag = 246
let closure_tag = 247
let object_tag = 248
let infix_tag = 249
let forward_tag = 250

let no_scan_tag = 251

let abstract_tag = 251
let string_tag = 252
let double_tag = 253
let double_array_tag = 254
let custom_tag = 255


let int_tag = 1000
let out_of_heap_tag = 1001
let unaligned_tag = 1002

(* module Closure = struct *)
(*   type info = { *)
(*     arity: int; *)
(*     start_env: int; *)
(*   } *)

(*   let info_of_raw (info : nativeint) = *)
(*     let open Nativeint in *)
(*     let arity = *)
(*       (\* signed: negative for tupled functions *\) *)
(*       if Sys.word_size = 64 then *)
(*         to_int (shift_right info 56) *)
(*       else *)
(*         to_int (shift_right info 24) *)
(*     in *)
(*     let start_env = *)
(*       (\* start_env is unsigned, but we know it can always fit an OCaml *)
(*          integer so we use [to_int] instead of [unsigned_to_int]. *\) *)
(*       to_int (shift_right_logical (shift_left info 8) 9) in *)
(*     { arity; start_env } *)

(*   (\* note: we expect a closure, not an infix pointer *\) *)
(*   let info (obj : t) = *)
(*     assert (tag obj = closure_tag); *)
(*     info_of_raw (raw_field obj 1) *)
(* end *)

module Extension_constructor =
struct
  type t = extension_constructor
  let of_val x =
    let x = repr x in
    let slot =
      if (is_block x) && (tag x) <> object_tag && (size x) >= 1 then field x 0
      else x
    in
    let name =
      if (is_block slot) && (tag slot) = object_tag then field slot 0
      else invalid_arg "Obj.extension_constructor"
    in
      if (tag name) = string_tag then (obj slot : t)
      else invalid_arg "Obj.extension_constructor"

  let [@inline always] name (slot : t) =
    (obj (field (repr slot) 0) : string)

  let [@inline always] id (slot : t) =
    (obj (field (repr slot) 1) : int)
end

end (* Obj *)

module Callback = struct

external register_named_value : string -> Obj.t -> unit
                              = "caml_register_named_value"

let register name v =
  register_named_value name (Obj.repr v)

let register_exception name (exn : exn) =
  let exn = Obj.repr exn in
  let slot = if Obj.tag exn = Obj.object_tag then exn else Obj.field exn 0 in
  register_named_value name slot
end (* Callback *)
