(* APPLE LOCAL file v7 support. Merge from Codesourcery *)
(* Auto-generate ARM Neon intrinsics header file.
   Copyright (C) 2006, 2007 Free Software Foundation, Inc.
   Contributed by CodeSourcery.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   This is an O'Caml program.  The O'Caml compiler is available from:

     http://caml.inria.fr/

   Or from your favourite OS's friendly packaging system. Tested with version
   3.09.2, though other versions will probably work too.
  
   Compile with:
     ocamlc -c neon.ml
     ocamlc -o neon-gen-std neon.cmo neon-gen-std.ml

   Run with:
     ./neon-gen-std > arm_neon_std.h
*)

open Neon

(* The format codes used in the following functions are documented at:
     http://caml.inria.fr/pub/docs/manual-ocaml/libref/Format.html\
     #6_printflikefunctionsforprettyprinting
   (one line, remove the backslash.)
*)

(* Following functions can be used to approximate GNU indentation style.  *)
let start_function () =
  Format.printf "@[<v 0>";
  ref 0

let end_function nesting =
  match !nesting with
    0 -> Format.printf "@;@;@]"
  | _ -> failwith ("Bad nesting (ending function at level "
                   ^ (string_of_int !nesting) ^ ")")
   
let open_braceblock nesting =
  begin match !nesting with
    0 -> Format.printf "@,@<0>{@[<v 2>@,"
  | _ -> Format.printf "@,@[<v 2>  @<0>{@[<v 2>@,"
  end;
  incr nesting

let close_braceblock nesting =
  decr nesting;
  match !nesting with
    0 -> Format.printf "@]@,@<0>}"
  | _ -> Format.printf "@]@,@<0>}@]"

(* LLVM LOCAL begin Print macros instead of inline functions.
   This is needed so that immediate arguments (e.g., lane numbers, shift
   amounts, etc.) can be checked for validity.  GCC can check them after
   inlining, but LLVM does inlining separately.

   Some macros translate to simple intrinsic calls and should not end with
   semicolons, but for others, which use GCC's statement-expressions to
   include unions that convert argument and/or return types, the semicolons
   need to be emitted after every statement.  This is implemented by deferring
   the emission of trailing semicolons so they are only added in the context
   of statement-expressions. *)
let print_function arity fnname body =
  let ffmt = start_function () in
  Format.printf "@[<v 2>#define ";
  begin match arity with
    Arity0 ret ->
      Format.printf "%s()" fnname
  | Arity1 (ret, arg0) ->
      Format.printf "%s(__a)" fnname
  | Arity2 (ret, arg0, arg1) ->
      Format.printf "%s(__a, __b)" fnname
  | Arity3 (ret, arg0, arg1, arg2) ->
      Format.printf "%s(__a, __b, __c)" fnname
  | Arity4 (ret, arg0, arg1, arg2, arg3) ->
      Format.printf "%s(__a, __b, __c, __d)" fnname
  end;
  let rec print_lines = function
    [] -> ()
  | [line] -> Format.printf "%s; \\" line
  | line::lines -> Format.printf "%s; \\@," line; print_lines lines in
  let print_macro_body = function
    [] -> Format.printf " \\@,";
  | [line] -> Format.printf " \\@,";
              Format.printf "%s" line
  | line::lines -> Format.printf " __extension__ \\@,";
                   Format.printf "@[<v 3>({ \\@,%s; \\@," line;
                   print_lines lines;
                   Format.printf "@]@, })" in
  print_macro_body body;
  Format.printf "@]";
  end_function ffmt
(* LLVM LOCAL end Print macros instead of inline functions.  *)

let return_by_ptr features = List.mem ReturnPtr features

let rec signed_ctype = function
    T_uint8x8 | T_poly8x8 -> T_int8x8
  | T_uint8x16 | T_poly8x16 -> T_int8x16
  | T_uint16x4 | T_poly16x4 -> T_int16x4
  | T_uint16x8 | T_poly16x8 -> T_int16x8
  | T_uint32x2 -> T_int32x2
  | T_uint32x4 -> T_int32x4
  | T_uint64x1 -> T_int64x1
  | T_uint64x2 -> T_int64x2
  (* Cast to types defined by mode in arm.c, not random types pulled in from
     the <stdint.h> header in use. This fixes incompatible pointer errors when
     compiling with C++.  *)
  | T_uint8 | T_int8 -> T_intQI
  | T_uint16 | T_int16 -> T_intHI
  | T_uint32 | T_int32 -> T_intSI
  | T_uint64 | T_int64 -> T_intDI
  | T_poly8 -> T_intQI
  | T_poly16 -> T_intHI
  | T_arrayof (n, elt) -> T_arrayof (n, signed_ctype elt)
  | T_ptrto elt -> T_ptrto (signed_ctype elt)
  | T_const elt -> T_const (signed_ctype elt)
  | x -> x

(* LLVM LOCAL begin union_string.
   Array types are handled as structs in llvm-gcc, not as wide integers, and
   single vector types have wrapper structs.  Unions are used here to convert
   back and forth between these different representations.  The union_string
   function has been updated accordingly, and it is moved below signed_ctype
   so it can use that function.  *)
let union_string num elts base =
  let itype = match num with
    1 -> elts
  | _ -> T_arrayof (num, elts) in
  let iname = string_of_vectype (signed_ctype itype)
  and sname = string_of_vectype itype in
  Printf.sprintf "union { %s __i; __neon_%s __o; } %s" sname iname base
(* LLVM LOCAL end union_string.  *)

(* LLVM LOCAL begin add_cast_with_prefix.  *)
let add_cast_with_prefix ctype cval stype_prefix =
  let stype = signed_ctype ctype in
  if ctype <> stype then
    match stype with
      T_ptrto elt ->
        Printf.sprintf "__neon_ptr_cast(%s%s, %s)" stype_prefix (string_of_vectype stype) cval
    | _ ->
        Printf.sprintf "(%s%s) %s" stype_prefix (string_of_vectype stype) cval
  else
    cval

let add_cast ctype cval = add_cast_with_prefix ctype cval ""
(* LLVM LOCAL end add_cast_with_prefix.  *)

let cast_for_return to_ty = "(" ^ (string_of_vectype to_ty) ^ ")"

(* Return a tuple of a list of declarations to go at the start of the function,
   and a list of statements needed to return THING.  *)
(* LLVM LOCAL begin Omit "return" keywords and trailing semicolons.  *)
let return arity return_by_ptr thing =
  match arity with
    Arity0 (ret) | Arity1 (ret, _) | Arity2 (ret, _, _) | Arity3 (ret, _, _, _)
  | Arity4 (ret, _, _, _, _) ->
    match ret with
      T_arrayof (num, vec) ->
        if return_by_ptr then
          let sname = string_of_vectype ret in
          [Printf.sprintf "%s __rv" sname],
          [thing; "__rv"]
        else
          let uname = union_string num vec "__rv" in
          [uname], ["__rv.__o = " ^ thing; "__rv.__i"]
    (* LLVM LOCAL begin Convert vector result to wrapper struct. *)
    | T_int8x8    | T_int8x16
    | T_int16x4   | T_int16x8
    | T_int32x2   | T_int32x4
    | T_int64x1   | T_int64x2
    | T_uint8x8   | T_uint8x16
    | T_uint16x4  | T_uint16x8
    | T_uint32x2  | T_uint32x4
    | T_uint64x1  | T_uint64x2
    | T_float32x2 | T_float32x4
    | T_poly8x8   | T_poly8x16
    | T_poly16x4  | T_poly16x8 ->
        let uname = union_string 1 ret "__rv" in
        [uname], ["__rv.__o = " ^ thing; "__rv.__i"]
    (* LLVM LOCAL end Convert vector result to wrapper struct. *)
    | T_void -> [], [thing]
    | _ ->
        [], [(cast_for_return ret) ^ thing]
(* LLVM LOCAL end Omit "return" keywords and trailing semicolons.  *)

let rec element_type ctype =
  match ctype with
    T_arrayof (_, v) -> element_type v
  | _ -> ctype

let params return_by_ptr ps =
  let pdecls = ref [] in
  let ptype t p =
    match t with
      T_arrayof (num, elts) ->
        let uname = union_string num elts (p ^ "u") in
        (* LLVM LOCAL Omit trailing semicolon.  *)
        let decl = Printf.sprintf "%s = { %s }" uname p in
        pdecls := decl :: !pdecls;
        p ^ "u.__o"
    (* LLVM LOCAL begin Extract vector operand from wrapper struct. *)
    | T_int8x8    | T_int8x16
    | T_int16x4   | T_int16x8
    | T_int32x2   | T_int32x4
    | T_int64x1   | T_int64x2
    | T_uint8x8   | T_uint8x16
    | T_uint16x4  | T_uint16x8
    | T_uint32x2  | T_uint32x4
    | T_uint64x1  | T_uint64x2
    | T_float32x2 | T_float32x4
    | T_poly8x8   | T_poly8x16
    | T_poly16x4  | T_poly16x8 ->
        let decl = Printf.sprintf "%s %s = %s"
          (string_of_vectype t) (p ^ "x") p in
        pdecls := decl :: !pdecls;
        add_cast_with_prefix t (p ^ "x.val") "__neon_"
    | T_immediate (lo, hi) -> p
    | _ ->
        let decl = Printf.sprintf "%s %s = %s"
          (string_of_vectype t) (p ^ "x") p in
        pdecls := decl :: !pdecls;
        add_cast t (p ^ "x") in
    (* LLVM LOCAL end Extract vector operand from wrapper struct. *)
  let plist = match ps with
    Arity0 _ -> []
  | Arity1 (_, t1) -> [ptype t1 "__a"]
  | Arity2 (_, t1, t2) -> [ptype t1 "__a"; ptype t2 "__b"]
  | Arity3 (_, t1, t2, t3) -> [ptype t1 "__a"; ptype t2 "__b"; ptype t3 "__c"]
  | Arity4 (_, t1, t2, t3, t4) ->
      [ptype t1 "__a"; ptype t2 "__b"; ptype t3 "__c"; ptype t4 "__d"] in
  match ps with
    Arity0 ret | Arity1 (ret, _) | Arity2 (ret, _, _) | Arity3 (ret, _, _, _)
  | Arity4 (ret, _, _, _, _) ->
      if return_by_ptr then
        !pdecls, add_cast (T_ptrto (element_type ret)) "&__rv.val[0]" :: plist
      else
        !pdecls, plist

let modify_params features plist =
  let is_flipped =
    List.exists (function Flipped _ -> true | _ -> false) features in
  if is_flipped then
    match plist with
      [ a; b ] -> [ b; a ]
    | _ ->
      failwith ("Don't know how to flip args " ^ (String.concat ", " plist))
  else
    plist

(* !!! Decide whether to add an extra information word based on the shape
   form.  *)
let extra_word shape features paramlist bits =
  let use_word =
    match shape with
      All _ | Long | Long_noreg _ | Wide | Wide_noreg _ | Narrow
    | By_scalar _ | Wide_scalar | Wide_lane | Binary_imm _ | Long_imm
    | Narrow_imm -> true
    | _ -> List.mem InfoWord features
  in
    if use_word then
      paramlist @ [string_of_int bits]
    else
      paramlist

(* Bit 0 represents signed (1) vs unsigned (0), or float (1) vs poly (0).
   Bit 1 represents rounding (1) vs none (0)
   Bit 2 represents floats & polynomials (1), or ordinary integers (0).  *)
let infoword_value elttype features =
  let bits02 =
    match elt_class elttype with
      Signed | ConvClass (Signed, _) | ConvClass (_, Signed) -> 0b001
    | Poly -> 0b100
    | Float -> 0b101
    | _ -> 0b000
  and rounding_bit = if List.mem Rounding features then 0b010 else 0b000 in
  bits02 lor rounding_bit

(* "Cast" type operations will throw an exception in mode_of_elt (actually in
   elt_width, called from there). Deal with that here, and generate a suffix
   with multiple modes (<to><from>).  *)
let rec mode_suffix elttype shape =
  try
    let mode = mode_of_elt elttype shape in
    string_of_mode mode
  with MixedMode (dst, src) ->
    let dstmode = mode_of_elt dst shape
    and srcmode = mode_of_elt src shape in
    string_of_mode dstmode ^ string_of_mode srcmode

let print_variant opcode features shape name (ctype, asmtype, elttype) =
  let bits = infoword_value elttype features in
  let modesuf = mode_suffix elttype shape in
  let return_by_ptr = return_by_ptr features in
  let pdecls, paramlist = params return_by_ptr ctype in
  let paramlist' = modify_params features paramlist in
  let paramlist'' = extra_word shape features paramlist' bits in
  let parstr = String.concat ", " paramlist'' in
  let builtin = Printf.sprintf "__builtin_neon_%s%s (%s)"
                  (builtin_name features name) modesuf parstr in
  let rdecls, stmts = return ctype return_by_ptr builtin in
  let body = pdecls @ rdecls @ stmts
  and fnname = (intrinsic_name name) ^ "_" ^ (string_of_elt elttype) in
  print_function ctype fnname body

(* When this function processes the element types in the ops table, it rewrites
   them in a list of tuples (a,b,c):
     a : C type as an "arity", e.g. Arity1 (T_poly8x8, T_poly8x8)
     b : Asm type : a single, processed element type, e.g. P16. This is the
         type which should be attached to the asm opcode.
     c : Variant type : the unprocessed type for this variant (e.g. in add
         instructions which don't care about the sign, b might be i16 and c
         might be s16.)
*)

let print_op (opcode, features, shape, name, munge, types) =
  let sorted_types = List.sort compare types in
  let munged_types = List.map
    (fun elt -> let c, asm = munge shape elt in c, asm, elt) sorted_types in
  List.iter
    (fun variant -> print_variant opcode features shape name variant)
    munged_types
  
let print_ops ops =
  List.iter print_op ops

(* Output type definitions. Table entries are:
     cbase : "C" name for the type.
     abase : "ARM" base name for the type (i.e. int in int8x8_t).
     esize : element size.
     enum : element count.
   We can't really distinguish between polynomial types and integer types in
   the C type system, I don't think, which may allow the user to make mistakes
   without warnings from the compiler.
   FIXME: It's probably better to use stdint.h names here.
*)

let deftypes () =
  let typeinfo = [
    (* Doubleword vector types.  *)
    "__builtin_neon_qi", "int", 8, 8;
    "__builtin_neon_hi", "int", 16, 4;
    "__builtin_neon_si", "int", 32, 2;
    "__builtin_neon_di", "int", 64, 1;
    "__builtin_neon_sf", "float", 32, 2;
    "__builtin_neon_poly8", "poly", 8, 8;
    "__builtin_neon_poly16", "poly", 16, 4;
    "__builtin_neon_uqi", "uint", 8, 8;
    "__builtin_neon_uhi", "uint", 16, 4;
    "__builtin_neon_usi", "uint", 32, 2;
    "__builtin_neon_udi", "uint", 64, 1;
    
    (* Quadword vector types.  *)
    "__builtin_neon_qi", "int", 8, 16;
    "__builtin_neon_hi", "int", 16, 8;
    "__builtin_neon_si", "int", 32, 4;
    "__builtin_neon_di", "int", 64, 2;
    "__builtin_neon_sf", "float", 32, 4;
    "__builtin_neon_poly8", "poly", 8, 16;
    "__builtin_neon_poly16", "poly", 16, 8;
    "__builtin_neon_uqi", "uint", 8, 16;
    "__builtin_neon_uhi", "uint", 16, 8;
    "__builtin_neon_usi", "uint", 32, 4;
    "__builtin_neon_udi", "uint", 64, 2
  ] in
  (* LLVM LOCAL remove typedefs for builtin Neon vector types *)
  (* Extra types not in <stdint.h>.  *)
  Format.printf "typedef __builtin_neon_sf float32_t;\n";
  Format.printf "typedef __builtin_neon_poly8 poly8_t;\n";
  Format.printf "typedef __builtin_neon_poly16 poly16_t;\n"
(* LLVM LOCAL begin Define containerized vector types. *)
  ;
  List.iter
    (fun (cbase, abase, esize, enum) ->
      let typename =
        Printf.sprintf "%s%dx%d_t" abase esize enum in
      let structname =
        Printf.sprintf "__simd%d_%s%d_t" (esize * enum) abase esize in
      let sfmt = start_function () in
      Format.printf "typedef struct %s" structname;
      open_braceblock sfmt;
      Format.printf "__neon_%s val;" typename;
      close_braceblock sfmt;
      Format.printf " %s;" typename;
      end_function sfmt)
    typeinfo
(* LLVM LOCAL end Define containerized vector types. *)

(* Output structs containing arrays, for load & store instructions etc.  *)

let arrtypes () =
  let typeinfo = [
    "int", 8;    "int", 16;
    "int", 32;   "int", 64;
    "uint", 8;   "uint", 16;
    "uint", 32;  "uint", 64;
    "float", 32; "poly", 8;
    "poly", 16
  ] in
  let writestruct elname elsize regsize arrsize =
    let elnum = regsize / elsize in
    let structname =
      Printf.sprintf "%s%dx%dx%d_t" elname elsize elnum arrsize in
    let sfmt = start_function () in
    Format.printf "typedef struct %s" structname;
    open_braceblock sfmt;
    Format.printf "%s%dx%d_t val[%d];" elname elsize elnum arrsize;
    close_braceblock sfmt;
    Format.printf " %s;" structname;
    end_function sfmt;
  in
    for n = 2 to 4 do
      List.iter
        (fun (elname, elsize) ->
          writestruct elname elsize 64 n;
          writestruct elname elsize 128 n)
        typeinfo
    done

let print_lines = List.iter (fun s -> Format.printf "%s@\n" s)

(* Do it.  *)

let _ =
  print_lines [
"/* Internal definitions for standard versions of NEON types and intrinsics.";
"   Do not include this file directly; please use <arm_neon.h>.";
"";
"   This file is generated automatically using neon-gen-std.ml.";
"   Please do not edit manually.";
"";
"   Copyright (C) 2006, 2007 Free Software Foundation, Inc.";
"   Contributed by CodeSourcery.";
"";
"   This file is part of GCC.";
"";
"   GCC is free software; you can redistribute it and/or modify it";
"   under the terms of the GNU General Public License as published";
"   by the Free Software Foundation; either version 2, or (at your";
"   option) any later version.";
"";
"   GCC is distributed in the hope that it will be useful, but WITHOUT";
"   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY";
"   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public";
"   License for more details.";
"";
"   You should have received a copy of the GNU General Public License";
"   along with GCC; see the file COPYING.  If not, write to the";
"   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,";
"   MA 02110-1301, USA.  */";
"";
"/* As a special exception, if you include this header file into source";
"   files compiled by GCC, this header file does not by itself cause";
"   the resulting executable to be covered by the GNU General Public";
"   License.  This exception does not however invalidate any other";
"   reasons why the executable file might be covered by the GNU General";
"   Public License.  */";
"";
"#ifndef _GCC_ARM_NEON_H";
"#define _GCC_ARM_NEON_H 1";
"";
"#ifndef __ARM_NEON__";
"#error You must enable NEON instructions (e.g. -mfloat-abi=softfp -mfpu=neon) to use arm_neon.h";
"#else";
"";
"#ifdef __cplusplus";
"extern \"C\" {";
(* LLVM LOCAL begin Use reinterpret_cast for pointers in C++ *)
"#define __neon_ptr_cast(ty, ptr) reinterpret_cast<ty>(ptr)";
"#else";
"#define __neon_ptr_cast(ty, ptr) (ty)(ptr)";
(* LLVM LOCAL end Use reinterpret_cast for pointers in C++ *)
"#endif";
"";
"#include <stdint.h>";
""];
  deftypes ();
  arrtypes ();
  Format.print_newline ();
  print_ops ops;
  Format.print_newline ();
  print_ops reinterp;
  print_lines [
"#ifdef __cplusplus";
"}";
"#endif";
"#endif";
"#endif"]
