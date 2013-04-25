#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=2 sw=2 expandtab:

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# -----------------------------------------------------
#
# High level intermediate Representation (HR) opcodes.
#
# Conventions:
# * Opcodes are in no particular order.
# * abc_ prefix means the opcode corresponds directly or almost
#   directly with an ABC OP_xxx instruction.  The mapping is not perfect
#   because of a few exceptions:
#     - abcbuilder expands some ABC opcodes that aren't possible to express
#       with templates, for example OP_callproperty's nullcheck is expanded
#       since a template can't receive and pass variadic arguments.
#     - sometimes multiple abc instructions map to a single HR instruction
#       for example OP_increment and OP_inclocal map to HR_abc_increment.
#     - sometimes a single abc instruction statically maps to multiple
#       HR instructions, for example OP_getproperty maps to one of four
#       HR instructions based on the multiname's signature.
# * names should be action verbs when it makes sense.
# 

hrdefs = '''

;;; PROTOTYPES

;; block delimiters 

; TopData and Top are used as wildcards in signatures. On the 
; use side, it means the required type is determined elsewhere,
; for example the constraint on goto inputs comes from the label.
; on the def side, it means the actual type is derived from uses
; (e.g. arm's def types come from if's uses) or a signature (start),
; or a-priori (const).

(start (-> Effect [Top])) ; start's defs include effect, state, env, and data
(template (-> Effect [Top]))
(catchblock (-> [Top]))

(return (Effect [Top] -> )) ; some templates may return vm types
(throw (Effect Atom -> )) ; throw only wants 1 data value even tho StopInstr allows N.

(goto ([Top] -> ))
(label ( -> [Top]))

(if (Boolean [Top] -> ))
(switch (Int [Top] -> ))
(arm ( -> [Top]))

;;; end block delimiters

;;; NOTE: don't use const explicitly in templates.
;;; instead just give the value (see e.g. abc_increment).
;;; The correct result type for explicit uses will NOT be
;;; calculated, currently.
;;;
(const (-> Top)) 

;;; NOTE: coerce and cast handled specially here - 
;;; second result type is based on second arg
(coerce (Effect Traits Atom -> Effect Atom))  

;;; cast is for downcasting.  Traits must be for a class type, not int/uint/number/string/bool/object.
(cast (Effect Traits Atom -> Effect ScriptObject))

;; castobject is for coerce<Object>.
;; fixme: what about ScriptObjects that don't extend Object?
(castobject (Atom -> Atom))

(tonumber (Effect Atom -> Effect Number)) 
(toint (Effect Atom -> Effect Int))  
(touint (Effect Atom -> Effect Uint))  
(toboolean (Atom -> Boolean))  
(d2b (Number -> Boolean)) ; specialize toboolean when input is Number
(caststring (Effect Atom -> Effect String))  
(castns (Effect Atom -> Effect Namespace))

(cknull (Effect Atom -> Effect Atom~))
(cknullobject (Effect ScriptObject -> Effect ScriptObject~))

(cktimeout (Effect Env -> Effect Boolean))             

(abc_hasnext (Effect Atom Int -> Effect Int))
(abc_hasnext2 (Effect Int Atom -> Effect Boolean Int Atom))

(never (-> Bot))

(call (Effect Atom Atom [Atom] -> Effect Atom))
(construct (Effect Atom Atom [Atom] -> Effect Atom~))
(constructsuper (Effect Env Atom~ [Atom] -> Effect Bot))

; access env various ways
(loadenv_namespace (Ord Namespace~ -> Env)) ; load MethodEnv* from Namespace
(loadenv_boolean (Ord Boolean -> Env)) ; load MethodEnv* from Boolean
(loadenv_number (Ord Number~ -> Env)) ; load MethodEnv* from Number
(loadenv_string (Ord String~ -> Env)) ; load MethodEnv* from String
(loadenv_interface (Method ScriptObject~ -> Env)) ; load MethodEnv* from Method IID
(loadenv (Ord ScriptObject~ -> Env)) ; load MethodEnv* from ScriptObject->vtable->methods[disp_id]
(loadenv_atom (Ord Atom~ -> Env)) ; load MethodEnv from toVTable(obj)->methods[disp_id]
(loadinitenv (ScriptObject~ -> Env)) ; load MethodEnv from ScriptObject->vtable->init
(loadsuperinitenv (Env -> Env)) ; load MethodEnv from env->vtable->base->init
(loadenv_env (Ord Env -> Env)) ; load MethodEnv from toVTable(obj)->methods[disp_id]

(newobject (Effect [Atom] -> Effect ScriptObject~)) ;;; TODO: result should be final with Object traits
(newarray (Effect [Atom] -> Effect Array~))   ;;; TODO: Array~ should be final, dense
(applytype (Effect [Atom] -> Effect Atom))
; fixme: input type is a non-null object whose traits has a nonnull itraits, e.g. a class
;       output type is the instance type of that class.
(newinstance (Class~ -> ScriptObject~))

(abc_convert_s (Effect Atom -> Effect String~))
(abc_esc_xelem (Effect Atom -> Effect String~))
(abc_esc_xattr (Effect Atom -> Effect String~))

(abc_typeof (Atom -> String~))

; Coerce to the given type or bailout
(speculate_int    (Atom State -> Int))
(speculate_number (Atom State -> Number))
(speculate_numeric (Atom State -> Number))
(speculate_string (Atom State -> String))
(speculate_object (Atom State -> Object))
(speculate_array (Atom State -> Array))
(speculate_bool (Atom State -> Boolean))


(ckfilter (Atom~ -> Atom~))  ; really *~[A] -> (XML|XMLList)[A]~

(abc_add (Effect Atom Atom -> Effect Atom)) ; OP_add.  really (Effect Atom Atom -> String~|Number)
(addd (Number Number -> Number))      ; LIR_addd
(concat_strings (String String -> String~))

(abc_nextname (Effect Atom Int -> Effect Atom))
(abc_nextvalue (Effect Atom Int -> Effect Atom))

(lessthan (Atom Atom -> Boolean))
(lessequals (Atom Atom -> Boolean))
(greaterthan (Atom Atom -> Boolean))
(greaterequals (Atom Atom -> Boolean))

(abc_instanceof (Effect Atom Atom -> Effect Boolean))
(abc_istype (Effect Traits Atom -> Effect Boolean))
(abc_istypelate (Effect Atom Atom~ -> Effect Boolean))
(abc_astype (Traits Atom -> Atom))
(abc_astypelate (Effect Atom Atom~ -> Effect Atom))  ; arg 2 is class, returns arg 1 of that type or null
(abc_in (Effect Atom Atom~ -> Effect Boolean))

(divd (Number Number -> Number))
(modulo (Number Number -> Number))
(subd (Number Number -> Number))
(muld (Number Number -> Number))

(addi (Int Int -> Int))
(subi (Int Int -> Int))
(muli (Int Int -> Int))

(ori (Int Int -> Int))
(andi (Int Int -> Int))
(xori (Int Int -> Int))

(lshi (Int Int -> Int))
(rshi (Int Int -> Int))
(rshui (Int Int -> Uint))

(noti (Int -> Int))
(negi (Int -> Int))

(negd (Number -> Number))
(not (Boolean -> Boolean))

(newactivation (Effect Env -> Effect ScriptObject~))

(abc_finddef (Effect Name Env -> Effect ScriptObject~))
(abc_findpropstrict (Effect Name Env Ord [Atom~] -> Effect Atom~))
(abc_findpropstrictx (Effect Name Env Ord Atom [Atom~] -> Effect Atom~))
(abc_findpropstrictns (Effect Name Env Ord Atom [Atom~] -> Effect Atom~))
(abc_findpropstrictnsx (Effect Name Env Ord Atom [Atom~] -> Effect Atom~))
(abc_findproperty (Effect Name Env Ord [Atom~] -> Effect Atom~))
(abc_findpropertyx (Effect Name Env Ord Atom [Atom~] -> Effect Atom~))
(abc_findpropertyns (Effect Name Env Ord Atom [Atom~] -> Effect Atom~))
(abc_findpropertynsx (Effect Name Env Ord Atom [Atom~] -> Effect Atom~))

(newclass (Effect Traits~ Class [Atom~] -> Effect Class~))  ; TODO vararg is list of Scope, need type
(newfunction (Effect Method [Atom~] -> Effect Function~))

(abc_getsuper (Effect Name Atom~ -> Effect Atom))              ; same as getprop inputs
(abc_getsuperx (Effect Name Atom Atom~ -> Effect Atom))        ; same as getpropx inputs
(abc_getsuperns (Effect Name Atom Atom~ -> Effect Atom))       ; same as getpropns inputs
(abc_getsupernsx (Effect Name Atom Atom Atom~ -> Effect Atom)) ; same as getpropnsx inputs

(abc_getdescendants    (Effect Name Atom~ -> Effect Atom))            ; same as getprop inputs
(abc_getdescendantsx   (Effect Name Atom Atom~ -> Effect Atom))       ; same as getprop inputs
(abc_getdescendantsns  (Effect Name Atom Atom~ -> Effect Atom))       ; same as getprop inputs
(abc_getdescendantsnsx (Effect Name Atom Atom Atom~ -> Effect Atom))  ; same as getprop inputs

(abc_callprop    (Effect Name Atom~ [Atom] -> Effect Atom))       ; for ABC OP_callproperty
(abc_callpropx   (Effect Name Atom Atom~ [Atom] -> Effect Atom))
(abc_callpropns  (Effect Name Atom Atom~ [Atom] -> Effect Atom))
(abc_callpropnsx (Effect Name Atom Atom Atom~ [Atom] -> Effect Atom))

(abc_callproplex    (Effect Name Atom~ [Atom] -> Effect Atom))
(abc_callproplexx   (Effect Name Atom Atom~ [Atom] -> Effect Atom))
(abc_callproplexns  (Effect Name Atom Atom~ [Atom] -> Effect Atom))
(abc_callproplexnsx (Effect Name Atom Atom Atom~ [Atom] -> Effect Atom))

(abc_constructprop    (Effect Name Atom~ [Atom] -> Effect Atom))   ; TODO check non-null result
(abc_constructpropx   (Effect Name Atom Atom~ [Atom] -> Effect Atom))   ; TODO check non-null result
(abc_constructpropns  (Effect Name Atom Atom~ [Atom] -> Effect Atom))   ; TODO check non-null result
(abc_constructpropnsx (Effect Name Atom Atom Atom~ [Atom] -> Effect Atom))   ; TODO check non-null result

(abc_callsuper    (Effect Name Atom~ [Atom] -> Effect Atom))
(abc_callsuperx   (Effect Name Atom Atom~ [Atom] -> Effect Atom))
(abc_callsuperns  (Effect Name Atom Atom~ [Atom] -> Effect Atom))
(abc_callsupernsx (Effect Name Atom Atom Atom~ [Atom] -> Effect Atom))

;; these are natively bound calls, they take TopData because
;; the real signature comes from the callee.
(callstatic (Effect Env TopData [TopData] -> Effect TopData)) 
(callmethod (Effect Env TopData [TopData] -> Effect TopData))
(callinterface (Effect Env TopData [TopData] -> Effect TopData))

(newcatch (Effect Traits~ -> Effect Atom~))
 
(setslot (Effect Ord ScriptObject~ TopData -> Effect Bot)) ; actual arg type is slot type
(getslot (Effect Ord ScriptObject~ -> Effect TopData))    ; actual result type is slot type
(slottype (ScriptObject~ Ord -> Traits))

(getouterscope (Ord Env -> Atom~)) 


(safepoint (Effect [State] -> Effect State))

(setlocal (State Atom -> State))  ; setlocal is only used to store state for exception edges, which require Atom

(newstate (-> State))                   ; create abstract VM state value

; DEOPT: new-style safepoints
(deopt_safepoint (Effect [TopData] -> Effect))
(deopt_finish (Effect -> Effect))
(deopt_finishcall (Effect TopData -> Effect))

; debug
(debugline (Effect Int -> Effect))
(debugfile (Effect String -> Effect))

; conversions to atom
(string2atom (String -> Atom))
(double2atom (Number -> Atom~))
(int2atom (Int -> Atom~))
(uint2atom (Uint -> Atom~))
(scriptobject2atom (ScriptObject -> Atom))
(bool2atom (Boolean -> Atom~))
(ns2atom (Namespace -> Atom))

; conversions from atom
(atom2bool (Atom~ -> Boolean))
(atom2double (Atom -> Number))
(atom2string (Atom -> String))
(atom2int (Atom -> Int))
(atom2uint (Atom -> Uint))
(atom2scriptobject (Atom -> ScriptObject))
(atom2ns (Atom -> Namespace))

; additional numeric conversions
(i2d (Int -> Number))
(u2d (Uint -> Number))
(d2i (Number -> Int)) ; should only be used when range is int32 already.
(d2u (Number -> Uint)) ; should only be used when range is uint32 already.

(toslot (ScriptObject Name -> Ord))

; TODO rename to to_primitive_atom?
; TODO need Primitive type
(toprimitive (Effect Atom -> Effect Atom))    

(eqi  (Int Int -> Boolean))              ; same as LIR_eqi
(lti  (Int Int -> Boolean))              ; same as LIR_lti
(lei  (Int Int -> Boolean))              ; same as LIR_lei
(gti  (Int Int -> Boolean))              ; same as LIR_gti
(gei  (Int Int -> Boolean))              ; same as LIR_gei
(eqd  (Number Number -> Boolean))        ; same as LIR_eqd
(ltd  (Number Number -> Boolean))        ; same as LIR_ltd
(led  (Number Number -> Boolean))        ; same as LIR_led
(gtd  (Number Number -> Boolean))        ; same as LIR_gtd
(ged  (Number Number -> Boolean))        ; same as LIR_ged
(equi (Uint Uint -> Boolean))            ; same as LIR_eqi
(ltui (Uint Uint -> Boolean))            ; same as LIR_ltui
(leui (Uint Uint -> Boolean))            ; same as LIR_leui
(gtui (Uint Uint -> Boolean))            ; same as LIR_gtui
(geui (Uint Uint -> Boolean))            ; same as LIR_geui
(eqb  (Boolean Boolean -> Boolean))      ; implemented with LIR_eqi

; value-modifying conversions
(doubletoint32 (Number -> Int))         ; ecma-262 compatible double->int
(i2u (Int -> Uint))                     ; coerce i2u (no-op at machine level)
(u2i (Uint -> Int))                     ; coerce u2i (no-op at machine level)

;;;;;;;;;;;;;;;;;;; TEMPLATES ;;;;;;;;;;;;;;;;;;

; unary instructions
(abc_increment    (x) (addd       (tonumber       x) 1.0))
(abc_decrement    (x) (subd       (tonumber       x) 1.0))
(abc_increment_i  (x) (addi       (toint          x) 1))
(abc_decrement_i  (x) (subi       (toint          x) 1))
(abc_not          (x) (not        (toboolean      x)))
(abc_negate       (x) (negd       (tonumber       x)))
(abc_bitnot       (x) (noti       (toint          x)))
(abc_negate_i     (x) (negi       (toint          x)))

(abc_sxi1         (x) (rshi (lshi (toint x) 31) 31))
(abc_sxi8         (x) (rshi (lshi (toint x) 24) 24))
(abc_sxi16        (x) (rshi (lshi (toint x) 16) 16))

(abc_dxnslate (Effect Atom -> Effect Bot)) ; calls dxns(core->intern(Atom))
(abc_dxns (Effect String~ -> Effect Bot))	; fixme: opcode table says these can throw, but can they?
(abc_checkfilter  (x) (ckfilter   (cknull x)))

; memory opcodes
(abc_li8       (addr)   (li8  (toint      addr)))
(abc_li16      (addr)   (li16 (toint      addr)))
(abc_li32      (addr)   (li32 (toint      addr)))
(abc_lf32      (addr)   (lf32 (toint      addr)))
(abc_lf64      (addr)   (lf64 (toint      addr)))
(abc_si8       (x addr) (si8  (toint    x) (toint addr)))
(abc_si16      (x addr) (si16 (toint    x) (toint addr)))
(abc_si32      (x addr) (si32 (toint    x) (toint addr)))
(abc_sf32      (x addr) (sf32 (tonumber x) (toint addr)))
(abc_sf64      (x addr) (sf64 (tonumber x) (toint addr)))

; mops primitives
(li8 (Effect Int -> Effect Int))
(li16 (Effect Int -> Effect Int))
(li32 (Effect Int -> Effect Int))
(lf32 (Effect Int -> Effect Number))
(lf64 (Effect Int -> Effect Number))
(si8 (Effect Int Int -> Effect Bot))
(si16 (Effect Int Int -> Effect Bot))
(si32 (Effect Int Int -> Effect Bot))
(sf32 (Effect Number Int -> Effect Bot))
(sf64 (Effect Number Int -> Effect Bot))

; binary instructions
(abc_subtract    (x y) (subd       (tonumber x) (tonumber y)))
(abc_multiply    (x y) (muld       (tonumber x) (tonumber y)))
(abc_modulo      (x y) (modulo     (tonumber x) (tonumber y)))
(abc_divide      (x y) (divd       (tonumber x) (tonumber y)))
(abc_add_i       (x y) (addi       (toint    x) (toint    y)))
(abc_subtract_i  (x y) (subi       (toint    x) (toint    y)))
(abc_multiply_i  (x y) (muli       (toint    x) (toint    y)))
(abc_bitor       (x y) (ori        (toint    x) (toint    y)))
(abc_bitand      (x y) (andi       (toint    x) (toint    y)))
(abc_bitxor      (x y) (xori       (toint    x) (toint    y)))
(abc_lshift      (x y) (lshi       (toint    x) (toint    y)))
(abc_rshift      (x y) (rshi       (toint    x) (toint    y)))
(abc_urshift     (x y) (rshui      (toint    x) (toint    y))) ; result is uint.

; specialized add
(add_ui (x y) (addd (u2d x) (i2d y)))
(add_nn (x y) (addd (tonumber x) (tonumber y)))
(add_ss (x y) (concat_strings (caststring x) (caststring y)))

; relational operators
(abc_lessthan (x y)
  (lessthan
    (toprimitive x)
    (toprimitive y)))
(abc_greaterequals (x y)
  (greaterequals
    (toprimitive x)
    (toprimitive y)))

; these call toprimitive out of order, see bugzilla 640052.
(abc_greaterthan (x y)
  (py = (toprimitive y))
  (px = (toprimitive x))
  (greaterthan px py))
(abc_lessequals (x y)
  (py = (toprimitive y))
  (px = (toprimitive x))
  (lessequals px py))

; specialized abc_equals
(abc_equals (Effect Atom Atom -> Effect Boolean))
(abc_equals_ii  (x y) (eqi x y))
(abc_equals_uu  (x y) (equi x y))
(abc_equals_bb  (x y) (eqb x y))
(abc_equals_nn  (x y) (eqd x y))
(abc_equals_pp  (x y) (eqp x y))
(abc_equals_ss  (x y) (eqs x y)) ; string comparison by value
(eqp (ScriptObject ScriptObject -> Boolean)) ; pointer comparison
(eqs (String String -> Boolean))

; TODO: if we have a common base type for ScriptObject|String|Namespace,
; (maybe RCObject), then the eqp primitive can take that and we can
; remove eqps.

(abc_strictequals (Atom Atom -> Boolean))

; other instructions

(abc_getslot (slot object)
  (getslot slot (cknullobject object)))

; abc_setsuper[x,ns,nsx]
(abc_setsuper    (Effect Name Atom~ Atom -> Effect Bot))
(abc_setsuperx   (Effect Name Atom Atom~ Atom -> Effect Bot))
(abc_setsuperns  (Effect Name Atom Atom~ Atom -> Effect Bot))
(abc_setsupernsx (Effect Name Atom Atom Atom~ Atom -> Effect Bot))

; abc_deleteprop[x,ns,nsx]
(abc_deleteprop    (Effect Name Atom~ -> Effect Boolean))        ; same as getprop inputs
(abc_deletepropx   (Effect Name Atom Atom~ -> Effect Boolean))  ; same as getpropx inputs
(abc_deletepropns  (Effect Name Atom Atom~ -> Effect Boolean))  ; same as getpropx inputs
(abc_deletepropnsx (Effect Name Atom Atom Atom~ -> Effect Boolean))  ; same as getpropx inputs

; abc_getprop
(abc_getprop (Effect Name Atom~ -> Effect Atom))
(abc_getprop_slot (name any) (abc_getslot (toslot any name) any))
  
; abc_getpropx
(abc_getpropx (Effect Name Atom Atom~ -> Effect Atom))              ; abc_getprop with [index]
(getpropx_u   (name index object) (getpropertylate_u index object)) ; abc_getpropx *[uint] -> *
(getpropx_i   (name index object) (getpropertylate_i index object)) ; abc_getpropx *[int] -> *
(getpropx_d   (name index object) (getpropertylate_d index object)) ; abc_getpropx *[double] -> *
(getpropx_au  (name index array)  (array_get_u index array))        ; abc_getpropx Array[uint] -> *
(getpropx_ai  (name index array)  (array_get_i index array))        ; abc_getpropx Array[int] -> *
(getpropx_ad  (name index array)  (array_get_d index array))        ; abc_getpropx Array[double] -> *
(getpropx_viu (name index vector) (vectorint_get_u index vector))   ; abc_getpropx Vector.<int>[uint] -> int
(getpropx_vii (name index vector) (vectorint_get_i index vector))   ; abc_getpropx Vector.<int>[int] -> int
(getpropx_vid (name index vector) (vectorint_get_d index vector))   ; abc_getpropx Vector.<int>[double] -> int
(getpropx_vuu (name index vector) (vectoruint_get_u index vector))   ; abc_getpropx Vector.<uint>[uint] -> uint
(getpropx_vui (name index vector) (vectoruint_get_i index vector))   ; abc_getpropx Vector.<uint>[int] -> uint
(getpropx_vud (name index vector) (vectoruint_get_d index vector))   ; abc_getpropx Vector.<uint>[double] -> uint
(getpropx_vdu (name index vector) (vectordouble_get_u index vector))   ; abc_getpropx Vector.<Number>[uint] -> Number
(getpropx_vdi (name index vector) (vectordouble_get_i index vector))   ; abc_getpropx Vector.<Number>[int] -> Number
(getpropx_vdd (name index vector) (vectordouble_get_d index vector))   ; abc_getpropx Vector.<Number>[double] -> Number
(getpropertylate_u (Effect Uint Atom~ -> Effect Atom))      ; MethodEnv.getpropertylate_u
(getpropertylate_i (Effect Int Atom~ -> Effect Atom))       ; MethodEnv.getpropertylate_i
(getpropertylate_d (Effect Number Atom~ -> Effect Atom))    ; MethodEnv.getpropertylate_d
(array_get_u (Effect Uint Array~ -> Effect Atom))           ; ArrayObject._getUintProperty
(array_get_i (Effect Int Array~ -> Effect Atom))            ; ArrayObject._getIntProperty
(array_get_d (Effect Number Array~ -> Effect Atom))         ; ArrayObject._getNumberProperty
(vectorint_get_u (Effect Uint VectorInt~ -> Effect Int))    ; IntVectorObject._getNativeUintProperty
(vectorint_get_i (Effect Int VectorInt~ -> Effect Int))     ; IntVectorObject._getNativeIntProperty
(vectorint_get_d (Effect Number VectorInt~ -> Effect Int))  ; IntVectorObject._getNativeNumberProperty
(vectoruint_get_u (Effect Uint VectorUInt~ -> Effect Uint))    ; UIntVectorObject._getNativeUintProperty
(vectoruint_get_i (Effect Int VectorUInt~ -> Effect Uint))     ; UIntVectorObject._getNativeIntProperty
(vectoruint_get_d (Effect Number VectorUInt~ -> Effect Uint))  ; UIntVectorObject._getNativeNumberProperty
(vectordouble_get_u (Effect Uint VectorDouble~ -> Effect Number))    ; DoubleVectorObject._getNativeUintProperty
(vectordouble_get_i (Effect Int VectorDouble~ -> Effect Number))     ; DoubleVectorObject._getNativeIntProperty
(vectordouble_get_d (Effect Number VectorDouble~ -> Effect Number))  ; DoubleVectorObject._getNativeNumberProperty

; abc_getpropns
(abc_getpropns (Effect Name Atom Atom~ -> Effect Atom))  ; abc_getprop with ns:: qualifier

; abc_getpropnsx
(abc_getpropnsx (Effect Name Atom Atom Atom~ -> Effect Atom))   ; name ns index object

; abc_setprop
(abc_setprop (Effect Name Atom~ Atom -> Effect Bot))
(abc_setprop_slot (name object val) (abc_setslot (toslot object name) object val)) 

; abc_setpropx
(abc_setpropx (Effect Name Atom Atom~ Atom -> Effect Bot))
(setpropx_u (name index object val) (setpropertylate_u index object val)) ; abc_setpropx *[uint]   <- *
(setpropx_i (name index object val) (setpropertylate_i index object val)) ; abc_setpropx *[int]    <- *
(setpropx_d (name index object val) (setpropertylate_d index object val)) ; abc_setpropx *[double] <- *
(setpropx_au (name index array val) (array_set_u index array val))        ; abc_setpropx Array[uint]   <- *
(setpropx_ai (name index array val) (array_set_i index array val))        ; abc_setpropx Array[int]    <- *
(setpropx_ad (name index array val) (array_set_d index array val))        ; abc_setpropx Array[double] <- *
(setpropx_viu (name index vector val) (vectorint_set_u index vector (toint val))) ; abc_setpropx Vector.<int>[uint]   <- *
(setpropx_vii (name index vector val) (vectorint_set_i index vector (toint val))) ; abc_setpropx Vector.<int>[int]    <- *
(setpropx_vid (name index vector val) (vectorint_set_d index vector (toint val))) ; abc_setpropx Vector.<int>[double] <- *
(setpropx_vuu (name index vector val) (vectoruint_set_u index vector (touint val))) ; abc_setpropx Vector.<uint>[uint]   <- *
(setpropx_vui (name index vector val) (vectoruint_set_i index vector (touint val))) ; abc_setpropx Vector.<uint>[int]    <- *
(setpropx_vud (name index vector val) (vectoruint_set_d index vector (touint val))) ; abc_setpropx Vector.<uint>[double] <- *
(setpropx_vdu (name index vector val) (vectordouble_set_u index vector (tonumber val))) ; abc_setpropx Vector.<Number>[uint]   <- *
(setpropx_vdi (name index vector val) (vectordouble_set_i index vector (tonumber val))) ; abc_setpropx Vector.<Number>[int]    <- *
(setpropx_vdd (name index vector val) (vectordouble_set_d index vector (tonumber val))) ; abc_setpropx Vector.<Number>[double] <- *
(setpropertylate_u (Effect Uint Atom~ Atom -> Effect Bot)) ; MethodEnv.setpropertylate_u
(setpropertylate_i (Effect Int Atom~ Atom -> Effect Bot)) ; MethodEnv.setpropertylate_i
(setpropertylate_d (Effect Number Atom~ Atom -> Effect Bot)) ; MethodEnv.setpropertylate_d
(array_set_u (Effect Uint Array~ Atom -> Effect Bot)) ; ArrayObject._setUintProperty
(array_set_i (Effect Int Array~ Atom -> Effect Bot)) ; ArrayObject._setIntProperty
(array_set_d (Effect Number Array~ Atom -> Effect Bot)) ; ArrayObject._setNumberProperty
(vectorint_set_u (Effect Uint VectorInt~ Int -> Effect Bot)) ; IntVectorObject._setNativeUintProperty
(vectorint_set_i (Effect Int VectorInt~ Int -> Effect Bot)) ; IntVectorObject._setNativeIntProperty
(vectorint_set_d (Effect Number VectorInt~ Int -> Effect Bot)) ; IntVectorObject._setNativeNumberProperty
(vectoruint_set_u (Effect Uint VectorUInt~ Uint -> Effect Bot)) ; UIntVectorObject._setNativeUintProperty
(vectoruint_set_i (Effect Int VectorUInt~ Uint -> Effect Bot)) ; UIntVectorObject._setNativeIntProperty
(vectoruint_set_d (Effect Number VectorUInt~ Uint -> Effect Bot)) ; UIntVectorObject._setNativeNumberProperty
(vectordouble_set_u (Effect Uint VectorDouble~ Number -> Effect Bot)) ; DoubleVectorObject._setNativeUintProperty
(vectordouble_set_i (Effect Int VectorDouble~ Number -> Effect Bot)) ; DoubleVectorObject._setNativeIntProperty
(vectordouble_set_d (Effect Number VectorDouble~ Number -> Effect Bot)) ; IntVectorObject._setNativeNumberProperty

; abc_setpropns
(abc_setpropns (Effect Name Atom Atom~ Atom -> Effect Bot))

; abc_setpropnsx
(abc_setpropnsx (Effect Name Atom Atom Atom~ Atom -> Effect Bot))

; abc_initprop
(abc_initprop    (Effect Name Atom~ Atom -> Effect Bot))            ; same as setprop inputs
(abc_initpropx   (Effect Name Atom Atom~ Atom -> Effect Bot))
(abc_initpropns  (Effect Name Atom Atom~ Atom -> Effect Bot))
(abc_initpropnsx (Effect Name Atom Atom Atom~ Atom -> Effect Bot))

; abc setslot
(abc_setslot (slot object val)
  (ptr = (cknullobject object))
  (setslot slot ptr (coerce (slottype ptr slot) val)))

; used when HR_find[prop,strict] is early bound to finddef.  scopes is not used.
(findprop2finddef (name env scopes)
  (abc_finddef env name))

; used when HR_find[prop,strict] is early bound to getouterscope. scopes is not used.
(findprop2getouter (index env scopes)
  (getouterscope index env))

; Used for builtin constructors, effect is implicit in abc_convert_s
; effect is implicit in abc_convert_s
(callprop_string (name stringclass value) (abc_convert_s value))

; specializations of coerce for different traits
(coerce_any     (traits value) value)
(coerce_object  (traits value) (castobject value))
(coerce_number  (traits value) (tonumber value))
(coerce_int     (traits value) (toint value))
(coerce_uint    (traits value) (touint value))
(coerce_boolean (traits value) (toboolean value))
(coerce_string  (traits value) (caststring value))
(coerce_ns      (traits value) (castns value))

; specializations of coerce for specific values and traits
(inttouint     (value) (i2u value))
(uinttoint     (value) (u2i value))
(numbertoint   (value) (doubletoint32 value))
(numbertouint  (value) (i2u (doubletoint32 value)))

; specializations of abc_modulo
(abc_modulo_and (x y) (i2u (andi (u2i x) (subi (u2i y) 1))))

'''

# ----------------------------------------------------------------------------------
#
# test decls
#
#

getpropx_tests = '''

; goal:
; len:uint = v.len
; if (uint(i) >= len) { call helper } // no else
; return v[i] // load

; v1: no phi node (if only traps; result discarded)
(getpropx_vii_v1 (name index vector)
  (len = (vectorint_len vector))
  (uix = (i2u index))
  (oob = (geui uix len))
  (if oob (vectorint_get_u uix vector) 0)
  (vectorint_load uix vector))

; v2: has phi (if traps or returns template result) 
(getpropx_vii_v2 (name index vector)
  (len = (vectorint_len vector))
  (uix = (i2u index))
  (oob = (geui uix len))
  (if oob 
    (vectorint_get_u uix vector) 
    (vectorint_load uix vector)))

; need these
(vectorint_len  (Effect VectorInt~ -> Effect Uint))
(vectorint_load (Effect Uint VectorInt~ -> Effect Int))

'''


flow_tests = '''

; test various combos of ifs and gotos

;;; IMPLICIT GOTOS

;;; simple if, implicit gotos only
(test_simple_if_implicit_gotos (c x y)
  (if c (abc_increment x) (abc_increment y)))

;;; nested if, implicit gotos only
(test_nested_if_implicit_gotos (c d x y)
  (if c
    (if d (abc_increment x) (abc_increment y))
    (if d (abc_decrement x) (abc_decrement y))))

;;; EXPLICIT GOTOS

;;; explicit goto (degenerate case)
(test_explicit_goto (x)
  (goto g x)
  (label g (x)
    (abc_increment x)))

;;; simple if, explicit gotos only
(test_simple_if_explicit_gotos (c x y)
  (if c (goto done x) (goto done y))
  (label done (z) (abc_increment z)))

;;; nested if, explicit gotos only
(test_nested_if_explicit_gotos (c d x y)
  (if c
    (if d (goto inc x) (goto inc y))
    (if d (goto dec x) (goto dec y)))
  (label inc (z) (abc_increment z))
  (label dec (z) (abc_decrement z)))

;;; nested if, explicit gotos only (2)
;;; here the labels are nested
;;;
(test_nested_if_explicit_gotos_2 (c d x y)
  (if c
    (true:
      (if d (goto done x) (goto done y))
      (label done (z) (abc_increment z)))
    (false:
      (if d (goto done x) (goto done y))
      (label done (z) (abc_decrement z)))))

;;; MIXED GOTOS

;;; simple if, mixed gotos
(test_simple_if_mixed_gotos (c x y)
  (if c (goto done x) (abc_decrement y))
  (label done (z) (abc_increment z)))

;;; nested if, mixed gotos
(test_nested_if_mixed_gotos (c d x y)
  (if c
    (if d (goto inc x) (abc_increment y))
    (if d (goto dec x) (abc_decrement y)))
  (label inc (z) (abc_increment z))
  (label dec (z) (abc_decrement z)))

;;; NON-TERMINAL IFS

;;; if as ternary subexpr
(test_nonterm_if_1 (c x y z)
  (abc_add z (if c (abc_increment x) (abc_decrement y))))

;;; if returns a value to binding
(test_nonterm_if_2 (c x y z)
  (temp = (if c (abc_increment x) (abc_decrement y)))
  (abc_add z temp))

;;; if computes unused values--questionable, but legal
(test_nonterm_if_3 (c x y z)
  (if c (abc_increment x) (abc_decrement y))
  (abc_add x y))

;;; errors: non-terminal ifs with gotos as arms

;;; if returns a value to binding
; (fail_nonterm_if_1 (c x y z)
;   (temp = (if c (goto inc x) (abc_decrement y)))
;   (abc_add z temp)
;   (label inc (n) (abc_increment n)))

;;; if computes unused values--questionable, but legal
; (fail_nonterm_if_2 (c x y z)
;   (if c (goto inc x) (abc_decrement y))
;   (abc_add x y)
;   (label inc (n) (abc_increment n)))


'''

# abc_add template test - addjovi version
#
abc_add_test = '''

; true if atom is kIntptrType
(isint (Atom -> Boolean))

; adds two ints, returns sum and overflow flag
(addjovi (Int Int Label -> Int))

; local defs version
(abc_add2 (x y)
  (if (isint x)
    (if (isint y) (addjovi x y slow) (goto slow))
    (goto slow))
  (label slow ()
    (abc_add x y)))

'''

# abc_add template test - multiple result version
#
abc_add_test_2 = '''

; true if atom is kIntptrType
(isint (Atom -> Boolean))

; adds two ints, returns sum and overflow flag
(addi_ov (Int Int -> Int Boolean))

; local defs version
(abc_add2 (x y)
  (if (isint x)
    (if (isint y) (goto fast) (goto slow))
    (goto slow))
  (label fast ()
    ((z ov) = (addi_ov (toint x) (toint y)))
    (if ov (goto slow) z))
  (label slow ()
    (abc_add x y)))

'''

# unit tests for if - more combinatorially exhaustive
# than flow_tests, but does not test interaction with
# goto.
#
if_tests = '''

; built-ins needed for any @if processing
(if (Boolean [Top] -> ))
(return (Effect [Top] -> )) ; some templates may return vm types

; misc used in tests 
(tonumber (Effect Atom -> Effect Number)) 
(lti  (Int Int -> Boolean))              ; same as LIR_lti
(negi (Int -> Int))
(addd (Number Number -> Number))      ; LIR_addd
(subd (Number Number -> Number))
(abc_increment    (x) (addd       (tonumber       x) 1.0))
(abc_decrement    (x) (subd       (tonumber       x) 1.0))
(cknull (Effect Atom -> Effect Atom~))

;
; structure
; 

; unnested short-form pure, returned immediately
(iftest0 (c x y) (if c x y))

; unnested short-form effectful, returned immediately
(iftest1a (c x) (if c (cknull x) x))
(iftest1b (a b x) (if (lti a b) (abc_increment x) (abc_decrement x)))
(iftest1c (x y) (if (lti x y) (negi x) (negi y)))

; unnested short-form as subx
(iftest2 (a b x z)
  (addd 
    (if (lti a b) (abc_increment x) (abc_decrement x))
    z))

; unnested short-form saved to local
(iftest3 (a b x z)
  (y = (if (lti a b) (abc_increment x) (abc_decrement x)))
  (addd y z))

;
; type inf
;

; simple
(iftest4a (c x y) (if c (negi x) y))
(iftest4b (c x y) (if c x (negi y)))

; nested 1
(iftest5a (b c x y z) (if b (if c (negi x) y) z))
(iftest5b (b c x y z) (if b (if c x (negi y)) z))
(iftest5c (b c x y z) (if b x (if c (negi y) z)))
(iftest5d (b c x y z) (if b x (if c y (negi z))))

; nested 2
(iftest6 (a b c w x y z)
  (if a
    (if b
      (if c w (negi x)) 
     y) 
   z))

;
; long form
; 

; multiple-statement bodies, immediate return
(iftest7 (c x y) 
  (if c 
    (true:
      (x1 = (abc_increment x))
      (y1 = (abc_decrement y))
      (addd x1 y1))
    (false:
      (x1 = (abc_decrement x))
      (y1 = (abc_increment y))
      (addd x1 y1))))

; multiple-statement bodies, bound to local
(iftest8 (c x y) 
  (z = 
    (if c 
      (true:
        (x1 = (abc_increment x))
        (y1 = (abc_decrement y))
        (addd x1 y1))
      (false:
        (x1 = (abc_decrement x))
        (y1 = (abc_increment y))
        (addd x1 y1))))
  z)


'''

