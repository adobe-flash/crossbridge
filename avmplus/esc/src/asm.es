/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Asm,
    namespace Asm;

/*******************************************************************
 * ABC constants
 */

const CONSTANT_Utf8               = 0x01;
const CONSTANT_Integer            = 0x03;
const CONSTANT_UInt               = 0x04;
const CONSTANT_PrivateNamespace   = 0x05;
const CONSTANT_Double             = 0x06;
const CONSTANT_QName              = 0x07; // ns::name, const ns, const name
const CONSTANT_Namespace          = 0x08;
const CONSTANT_Multiname          = 0x09; // [ns...]::name, const [ns...], const name
const CONSTANT_False              = 0x0A;
const CONSTANT_True               = 0x0B;
const CONSTANT_Null               = 0x0C;
const CONSTANT_QNameA             = 0x0D; // @ns::name, const ns, const name
const CONSTANT_MultinameA         = 0x0E; // @[ns...]::name, const [ns...], const name
const CONSTANT_RTQName            = 0x0F; // ns::name, var ns, const name
const CONSTANT_RTQNameA           = 0x10; // @ns::name, var ns, const name
const CONSTANT_RTQNameL           = 0x11; // ns::[name], var ns, var name
const CONSTANT_RTQNameLA          = 0x12; // @ns::[name], var ns, var name
const CONSTANT_NameL              = 0x13; // o[name], var name
const CONSTANT_NameLA             = 0x14; // @[name], var name
const CONSTANT_NamespaceSet       = 0x15;
const CONSTANT_PackageNamespace   = 0x16; // namespace for a package
const CONSTANT_PackageInternalNS  = 0x17;
const CONSTANT_ProtectedNamespace = 0x18;
const CONSTANT_ExplicitNamespace  = 0x19;
const CONSTANT_StaticProtectedNS  = 0x1A;
const CONSTANT_MultinameL         = 0x1B;
const CONSTANT_MultinameLA        = 0x1C;

const CONSTANT_ClassSealed        = 0x01;
const CONSTANT_ClassFinal         = 0x02;
const CONSTANT_ClassInterface     = 0x04;
const CONSTANT_ClassProtectedNs   = 0x08;

const TRAIT_Slot                  = 0;
const TRAIT_Method                = 1;
const TRAIT_Getter                = 2;
const TRAIT_Setter                = 3;
const TRAIT_Class                 = 4;
const TRAIT_Function              = 5;
const TRAIT_Const                 = 6;

const ATTR_Final                  = 0x01;
const ATTR_Override               = 0x02;
const ATTR_Metadata               = 0x04;

const SLOT_var                    = 0;
const SLOT_method                 = 1;
const SLOT_getter                 = 2;
const SLOT_setter                 = 3;
const SLOT_class                  = 4;
const SLOT_function               = 6;

const METHOD_Arguments            = 0x1;
const METHOD_Activation           = 0x2;
const METHOD_Needrest             = 0x4;
const METHOD_HasOptional          = 0x8;
const METHOD_IgnoreRest           = 0x10;
const METHOD_Native               = 0x20;
const METHOD_Setsdxns             = 0x40;
const METHOD_HasParamNames        = 0x80;

const OP_bkpt:int = 0x01;
const OP_nop:int = 0x02;
const OP_throw:int = 0x03;
const OP_getsuper:int = 0x04;
const OP_setsuper:int = 0x05;
const OP_dxns:int = 0x06;
const OP_dxnslate:int = 0x07;
const OP_kill:int = 0x08;
const OP_label:int = 0x09;
const OP_ifnlt:int = 0x0C;
const OP_ifnle:int = 0x0D;
const OP_ifngt:int = 0x0E;
const OP_ifnge:int = 0x0F;
const OP_jump:int = 0x10;
const OP_iftrue:int = 0x11;
const OP_iffalse:int = 0x12;
const OP_ifeq:int = 0x13;
const OP_ifne:int = 0x14;
const OP_iflt:int = 0x15;
const OP_ifle:int = 0x16;
const OP_ifgt:int = 0x17;
const OP_ifge:int = 0x18;
const OP_ifstricteq:int = 0x19;
const OP_ifstrictne:int = 0x1A;
const OP_lookupswitch:int = 0x1B;
const OP_pushwith:int = 0x1C;
const OP_popscope:int = 0x1D;
const OP_nextname:int = 0x1E;
const OP_hasnext:int = 0x1F;
const OP_pushnull:int = 0x20;
const OP_pushundefined:int = 0x21;
const OP_pushconstant:int = 0x22;
const OP_nextvalue:int = 0x23;
const OP_pushbyte:int = 0x24;
const OP_pushshort:int = 0x25;
const OP_pushtrue:int = 0x26;
const OP_pushfalse:int = 0x27;
const OP_pushnan:int = 0x28;
const OP_pop:int = 0x29;
const OP_dup:int = 0x2A;
const OP_swap:int = 0x2B;
const OP_pushstring:int = 0x2C;
const OP_pushint:int = 0x2D;
const OP_pushuint:int = 0x2E;
const OP_pushdouble:int = 0x2F;
const OP_pushscope:int = 0x30;
const OP_pushnamespace:int = 0x31;
const OP_hasnext2:int = 0x32;
const OP_newfunction:int = 0x40;
const OP_call:int = 0x41;
const OP_construct:int = 0x42;
const OP_callmethod:int = 0x43;
const OP_callstatic:int = 0x44;
const OP_callsuper:int = 0x45;
const OP_callproperty:int = 0x46;
const OP_returnvoid:int = 0x47;
const OP_returnvalue:int = 0x48;
const OP_constructsuper:int = 0x49;
const OP_constructprop:int = 0x4A;
const OP_callsuperid:int = 0x4B;
const OP_callproplex:int = 0x4C;
const OP_callinterface:int = 0x4D;
const OP_callsupervoid:int = 0x4E;
const OP_callpropvoid:int = 0x4F;
const OP_newobject:int = 0x55;
const OP_newarray:int = 0x56;
const OP_newactivation:int = 0x57;
const OP_newclass:int = 0x58;
const OP_getdescendants:int = 0x59;
const OP_newcatch:int = 0x5A;
const OP_findpropstrict:int = 0x5D;
const OP_findproperty:int = 0x5E;
const OP_finddef:int = 0x5F;
const OP_getlex:int = 0x60;
const OP_setproperty:int = 0x61;
const OP_getlocal:int = 0x62;
const OP_setlocal:int = 0x63;
const OP_getglobalscope:int = 0x64;
const OP_getscopeobject:int = 0x65;
const OP_getproperty:int = 0x66;
const OP_getouterscope:int = 0x67;
const OP_initproperty:int = 0x68;
const OP_setpropertylate:int = 0x69;
const OP_deleteproperty:int = 0x6A;
const OP_deletepropertylate:int = 0x6B;
const OP_getslot:int = 0x6C;
const OP_setslot:int = 0x6D;
const OP_getglobalslot:int = 0x6E;
const OP_setglobalslot:int = 0x6F;
const OP_convert_s:int = 0x70;
const OP_esc_xelem:int = 0x71;
const OP_esc_xattr:int = 0x72;
const OP_convert_i:int = 0x73;
const OP_convert_u:int = 0x74;
const OP_convert_d:int = 0x75;
const OP_convert_b:int = 0x76;
const OP_convert_o:int = 0x77;
const OP_checkfilter:int = 0x78;
const OP_coerce:int = 0x80;
const OP_coerce_b:int = 0x81;
const OP_coerce_a:int = 0x82;
const OP_coerce_i:int = 0x83;
const OP_coerce_d:int = 0x84;
const OP_coerce_s:int = 0x85;
const OP_astype:int = 0x86;
const OP_astypelate:int = 0x87;
const OP_coerce_u:int = 0x88;
const OP_coerce_o:int = 0x89;
const OP_negate:int = 0x90;
const OP_increment:int = 0x91;
const OP_inclocal:int = 0x92;
const OP_decrement:int = 0x93;
const OP_declocal:int = 0x94;
const OP_typeof:int = 0x95;
const OP_not:int = 0x96;
const OP_bitnot:int = 0x97;
const OP_concat:int = 0x9A;
const OP_add_d:int = 0x9B;
const OP_add:int = 0xA0;
const OP_subtract:int = 0xA1;
const OP_multiply:int = 0xA2;
const OP_divide:int = 0xA3;
const OP_modulo:int = 0xA4;
const OP_lshift:int = 0xA5;
const OP_rshift:int = 0xA6;
const OP_urshift:int = 0xA7;
const OP_bitand:int = 0xA8;
const OP_bitor:int = 0xA9;
const OP_bitxor:int = 0xAA;
const OP_equals:int = 0xAB;
const OP_strictequals:int = 0xAC;
const OP_lessthan:int = 0xAD;
const OP_lessequals:int = 0xAE;
const OP_greaterthan:int = 0xAF;
const OP_greaterequals:int = 0xB0;
const OP_instanceof:int = 0xB1;
const OP_istype:int = 0xB2;
const OP_istypelate:int = 0xB3;
const OP_in:int = 0xB4;
const OP_increment_i:int = 0xC0;
const OP_decrement_i:int = 0xC1;
const OP_inclocal_i:int = 0xC2;
const OP_declocal_i:int = 0xC3;
const OP_negate_i:int = 0xC4;
const OP_add_i:int = 0xC5;
const OP_subtract_i:int = 0xC6;
const OP_multiply_i:int = 0xC7;
const OP_getlocal0:int = 0xD0;
const OP_getlocal1:int = 0xD1;
const OP_getlocal2:int = 0xD2;
const OP_getlocal3:int = 0xD3;
const OP_setlocal0:int = 0xD4;
const OP_setlocal1:int = 0xD5;
const OP_setlocal2:int = 0xD6;
const OP_setlocal3:int = 0xD7;
const OP_debug:int = 0xEF;
const OP_debugline:int = 0xF0;
const OP_debugfile:int = 0xF1;
const OP_bkptline:int = 0xF2;
const OP_timestamp:int = 0xF3;

/*********************************************************************************
 * AVM2 assembler for one code block.
 *
 * This is a lightweight class that is used to emit bytes for
 * instructions and data, to maintain stack and scope depths,
 * count local slots used, and to handle branch targets and
 * backpatching.  It has no code generation logic save for fairly
 * simple abstractions (eg, I_getlocal() maps to "getlocal_n" or
 * to the general "getlocal" instruction, depending on its
 * parameter value).
 *
 * FIXME:
 *  - There needs to be a way to set the scope stack depth to 0, to be used
 *    when generating code for exception handling
 *  - It would be nice if we could check that every join point has the same
 *    stack depth, this requires that the next linear instruction following
 *    an unconditional nonreturning control flow (return, throw, jump) is
 *    a label always, or is ignored for the purposes of computing the stack
 *    depth.
 *  - Ditto for the scope depth, really.
 */

var listify = false;

class AVM2Assembler
{
    const indent = "        ";

    private var code = new ABCByteStream;
    private var nextLabel = 1000;
    private var backpatches = [];
    private var current_scope_depth = 0;
    private var max_scope_depth = 0;
    private var current_stack_depth = 0;
    private var max_stack_depth = 0;
    private var nextTemp;
    private var freeTemps = [];
    private var constants;
    private var set_dxns = false;
    private var need_activation = false;
    private var attr = null;

    function AVM2Assembler(constants, numberOfFormals, attr)
        : constants = constants
        , current_scope_depth = 0
        , attr = attr
        , nextTemp = numberOfFormals + 1 + (attr.uses_arguments || attr.uses_rest ? 1 : 0) // local 0 is always "this"
    {
    }

    function get maxStack() { return max_stack_depth }
    function get maxLocal() { return nextTemp }
    function get maxScope() { return max_scope_depth }
    function get flags() { 
        var f = 0;
        if (set_dxns)
            f |= METHOD_Setsdxns;
        if (need_activation)
            f |= METHOD_Activation;
        if (attr.uses_arguments)
            f |= METHOD_Arguments;
        if (attr.uses_rest)
            f |= METHOD_Needrest;
        return f;
    }

    private function listL(n) {
        if (listify)
            print(n);
    }

    private function list1(name) {
        if (listify)
            print(indent + name);
    }

    private function list2(name, v) {
        if (listify)
            print(indent + name + " " + v);
    }

    private function list3(name, v1, v2) {
        if (listify)
            print(indent + name + " " + v1 + " " + v2);
    }

    private function list5(name, v1, v2, v3, v4) {
        if (listify)
            print(indent + name + " " + v1 + " " + v2 + " " + v3 + " " + v4);
    }
    /*         function listn(name, ...rest) {
               if (listify)
               print(indent + name + " " + rest.join(" "));
               }
    */

    // Instructions that push one value, with a single opcode byte
    private function pushOne(name, opcode) {
        stack(1);
        list1(name);
        code.uint8(opcode);
    }

    function I_dup() { pushOne("dup", 0x2A) }
    function I_getglobalscope() { pushOne("getglobalscope", 0x64) }
    function I_getlocal_0() { pushOne("getlocal_0", 0xD0) }
    function I_getlocal_1() { pushOne("getlocal_1", 0xD1) }
    function I_getlocal_2() { pushOne("getlocal_2", 0xD2) }
    function I_getlocal_3() { pushOne("getlocal_3", 0xD3) }
    function I_newactivation() { need_activation=true; pushOne("newactivation", 0x57) }
    function I_pushfalse() { pushOne("pushfalse", 0x27) }
    function I_pushnan() { pushOne("pushnan", 0x28) }
    function I_pushnull() { pushOne("pushnull", 0x20) }
    function I_pushtrue() { pushOne("pushtrue", 0x26) }
    function I_pushundefined() { pushOne("pushundefined", 0x21) }

    // Instructions that push one value, with an opcode byte followed by a u30 argument
    private function pushOneU30(name, opcode, v) {
        stack(1);
        list2(name, v);
        code.uint8(opcode);
        code.uint30(v);
    }

    function I_getglobalslot(index) { pushOneU30("getglobalslot", 0x6E, index) }
    function I_getlex(index) { pushOneU30("getlex", 0x60, index) }
    function I_getscopeobject(index) { pushOneU30("getscopeobject", 0x65, index) }
    function I_getouterscope(index) { pushOneU30("getouterscope", 0x67, index) }
    function I_newcatch(index) { pushOneU30("newcatch", 0x5A, index) }
    function I_newfunction(index) { pushOneU30("newfunction", 0x40, index) }
    function I_pushdouble(index) { pushOneU30("pushdouble", 0x2F, index) }
    function I_pushint(index) { pushOneU30("pushint", 0x2D, index) }
    function I_pushnamespace(index) { pushOneU30("pushnamespace", 0x31, index) }
    function I_pushshort(v) { pushOneU30("pushshort", 0x25, v) }
    function I_pushstring(index) { pushOneU30("pushstring", 0x2C, index) }
    function I_pushuint(index) { pushOneU30("pushuint", 0x2E, index) }

    // start a catch block.  increments stack by 1 for the exception object
    function startCatch() { stack(1) }
        
    // Instructions that pop one value, with a single opcode byte
    private function dropOne(name, opcode) {
        stack(-1);
        list1(name);
        code.uint8(opcode);
    }

    function I_add() { dropOne("add", 0xA0) }
    function I_add_i() { dropOne("add_i", 0xC5) }
    function I_astypelate() { dropOne("astypelate", 0x87) }
    function I_bitand() { dropOne("bitand", 0xA8) }
    function I_bitor() { dropOne("bitor", 0xA9) }
    function I_bitxor() { dropOne("bitxor", 0xAA) }
    function I_divide() { dropOne("divide", 0xA3) }
    function I_dxnslate() { set_dxns=true; dropOne("dxnslate", 0x07) }
    function I_equals() { dropOne("Equals", 0xAB) }
    function I_greaterequals() { dropOne("greaterequals", 0xB0) }
    function I_greaterthan() { dropOne("greaterthan", 0xAF) }
    function I_hasnext() { dropOne("hasnext", 0x1F) }
    function I_in() { dropOne("in", 0xB4) }
    function I_instanceof() { dropOne("instanceof", 0xB1) }
    function I_istypelate() { dropOne("istypelate", 0xB3) }
    function I_lessequals() { dropOne("lessequals", 0xAE) }
    function I_lessthan() { dropOne("lessthan", 0xAD) }
    function I_lshift() { dropOne("lshift", 0xA5) }
    function I_modulo() { dropOne("modulo", 0xA4) }
    function I_multiply() { dropOne("multiply", 0xA2) }
    function I_multiply_i() { dropOne("multiply_i", 0xC7) }
    function I_nextname() { dropOne("nextname", 0x1E) }
    function I_nextvalue() { dropOne("nextvalue", 0x23) }
    function I_pop() { dropOne("pop", 0x29) }
    function I_pushscope() { scope(1); dropOne("pushscope", 0x30) }
    function I_pushwith() { scope(1); dropOne("pushwith", 0x1C) }
    function I_returnvalue() { dropOne("returnvalue", 0x48) }
    function I_rshift() { dropOne("rshift", 0xA6) }
    function I_setlocal_0() { dropOne("setlocal_0", 0xD4) }
    function I_setlocal_1() { dropOne("setlocal_1", 0xD5) }
    function I_setlocal_2() { dropOne("setlocal_2", 0xD6) }
    function I_setlocal_3() { dropOne("setlocal_3", 0xD7) }
    function I_strictequals() { dropOne("strictequals", 0xAC) }
    function I_subtract() { dropOne("subtract", 0xA1) }
    function I_subtract_i() { dropOne("subtract_i", 0xC6) }
    function I_throw() { dropOne("throw", 0x03) }
    function I_urshift() { dropOne("urshift", 0xA7) }

    // Instructions that pop one value, with an opcode byte followed by an u30 argument
    private function dropOneU30(name, opcode, v) {
        stack(-1);
        list2(name, v);
        code.uint8(opcode);
        code.uint30(v);
    }

    function I_setglobalslot(index) { dropOneU30("setglobalslot", 0x6F, index) }

    // Instructions that do not change the stack height, with a single opcode byte
    private function dropNone(name, opcode)
    {
        //stack(0);
        list1(name);
        code.uint8(opcode);
    }

    function I_bitnot() { dropNone("bitnot", 0x97) }
    function I_checkfilter() { dropNone("checkfilter", 0x78) }
    function I_coerce_a() { dropNone("coerce_a", 0x82) }
    function I_coerce_s() { dropNone("coerce_s", 0x85) }
    function I_convert_b() { dropNone("convert_b", 0x76) }
    function I_convert_d() { dropNone("convert_d", 0x75) }
    function I_convert_i() { dropNone("convert_i", 0x73) }
    function I_convert_o() { dropNone("convert_o", 0x77) }
    function I_convert_s() { dropNone("convert_s", 0x70) }
    function I_convert_u() { dropNone("convert_u", 0x74) }
    function I_decrement() { dropNone("decrement", 0x93) }
    function I_decrement_i() { dropNone("decrement_i", 0xC1) }
    function I_esc_xattr() { dropNone("esc_xattr", 0x72) }
    function I_esc_xelem() { dropNone("esc_xattr", 0x71) }
    function I_increment() { dropNone("increment", 0x91) }
    function I_increment_i() { dropNone("increment_i", 0xC0) }
    function I_negate() { dropNone("negate", 0x90) }
    function I_negate_i() { dropNone("negate_i", 0xC4) }
    function I_nop() { dropNone("nop", 0x02) }
    function I_not() { dropNone("not", 0x96) }
    function I_popscope() { scope(-1); dropNone("popscope", 0x1D) }
    function I_returnvoid() { dropNone("returnvoid", 0x47) }
    function I_swap() { dropNone("swap", 0x2B) }
    function I_typeof() { dropNone("typeof", 0x95) }

    // Instructions that do not change the stack height, with an opcode byte
    // followed by a u30 argument
    private function dropNoneU30(name, opcode, x) {
        //stack(0)
        list2(name, x);
        code.uint8(opcode);
        code.uint30(x);
    }

    function I_astype(index) { dropNoneU30("astype", 0x86, index) }
    function I_coerce(index) { dropNoneU30("coerce", 0x80, index) }
    function I_debugfile(index) { dropNoneU30("debugfile", 0xF1, index) }
    function I_debugline(linenum) { dropNoneU30("debugline", 0xF0, linenum) }
    function I_declocal(reg) { dropNoneU30("declocal", 0x94, reg) }
    function I_declocal_i(reg) { dropNoneU30("declocal_i", 0xC3, reg) }
    function I_dxns(index) { set_dxns=true; dropNoneU30("dxns", 0x06, index) }
    function I_getslot(index) { dropNoneU30("getslot", 0x6C, index) }
    function I_inclocal(reg) { dropNoneU30("inclocal", 0x92, reg) }
    function I_inclocal_i(reg) { dropNoneU30("inclocal_i", 0xC2, reg) }
    function I_istype(index) { dropNoneU30("istype", 0xB2, index) }
    function I_kill(index) { dropNoneU30("kill", 0x08, index) }
    function I_newclass(index) { dropNoneU30("newclass", 0x58, index) }

    function I_getlocal(index) {
        switch (index) {
        case 0: I_getlocal_0(); break;
        case 1: I_getlocal_1(); break;
        case 2: I_getlocal_2(); break;
        case 3: I_getlocal_3(); break;
        default: pushOneU30("getlocal", 0x62, index);
        }
    }

    function I_setlocal(index) {
        switch (index) {
        case 0: I_setlocal_0(); break;
        case 1: I_setlocal_1(); break;
        case 2: I_setlocal_2(); break;
        case 3: I_setlocal_3(); break;
        default: dropOneU30("setlocal", 0x63, index);
        }
    }

    // Local control flow instructions and I_label():
    //  - If called without an argument return a "label" that can later be
    //    passed to I_label() to give the label an actual value.
    //  - If called with an argument, the argument must have been returned
    //    from a control flow instruction or from I_label().  It represents
    //    a transfer target.
    //
    // A "label" is a data structure with these fields:
    //  - name (uint): a symbolic name for the label, to be used in listings
    //  - address (int): either -1 for "unknown" or the address of the label
    //  - stack (uint): the stack depth at label creation time; this is the
    //        stack depth at the target too [except for exception handling]
    //  - scope (uint): the scope stack depth at label creation time; this is the
    //        scope stack depth at the target too [except for exception handling]
    //
    // The method newLabel() can be called to return a label that
    // will later be defined by I_label and referenced by control
    // flow instructions, without creating a jump instruction at
    // the point where the label is created.  Typically this is
    // used to create branch targets for "break" and "continue".

    function newLabel() {
        return { "name": nextLabel++, "address": -1, "stack": current_stack_depth, "scope": current_scope_depth };
    }

    private function relativeOffset(base, L) {
        if (L.address != -1)
            code.int24(L.address - base);
        else {
            backpatches.push({ "loc": code.length, "base": base, "label": L });
            code.int24(0);
        }
    }

    private function jmp(stk, name, opcode, L) {
        stack(stk);

        if (L === undefined)
            L = newLabel();

        list2(name, L.name);
        code.uint8(opcode);
        relativeOffset(code.length+3, L);

        return L;
    }

    function I_label(L) {
        var here = code.length;
        var define = false;
        if (L === undefined) {
            define = true;
            L = newLabel();
        }
        else {
            Util::assert( L.address == -1 );
            current_stack_depth = L.stack;
            current_scope_depth = L.scope;
        }
        L.address = here;
        listL(L.name + ":   -- " + L.stack + "/" + L.scope);
        if (define) {
            code.uint8(0x09);
            list1("label");
        }
        return L;
    }

    function I_ifeq(L) { return jmp(-2, "ifeq", 0x13, L) }
    function I_ifge(L) { return jmp(-2, "ifge", 0x18, L) }
    function I_ifgt(L) { return jmp(-2, "ifgt", 0x17, L) }
    function I_ifle(L) { return jmp(-2, "ifle", 0x16, L) }
    function I_iflt(L) { return jmp(-2, "iflt", 0x15, L) }
    function I_ifne(L) { return jmp(-2, "ifne", 0x14, L) }
    function I_ifnge(L) { return jmp(-2, "ifnge", 0x0F, L) }
    function I_ifngt(L) { return jmp(-2, "ifngt", 0x0E, L) }
    function I_ifnle(L) { return jmp(-2, "ifnle", 0x0D, L) }
    function I_ifnlt(L) { return jmp(-2, "ifnlt", 0x0C, L) }
    function I_ifstricteq(L) { return jmp(-2, "ifstricteq", 0x19, L) }
    function I_ifstrictne(L) { return jmp(-2, "ifstrictne", 0x1A, L) }

    function I_iffalse(L) { return jmp(-1, "iffalse", 0x12, L) }
    function I_iftrue(L) { return jmp(-1, "iftrue", 0x11, L) }

    function I_jump(L) { return jmp(0, "jump", 0x10, L) }

    // Here, case_labels must be an array with a "length" property
    // that denotes the number of case labels in the array.
    // length cannot be 0.
    //
    // Either default_label is undefined and all the elements of
    // case_labels are also undefined, or default_label is a label
    // structure, and all the elements of case_labels between 0
    // and length-1 are label structures as well.
    //
    // In the former case, labels are created for the
    // default_label and for all the case_labels; the array is
    // updated; and the new default_label is returned.

    function I_lookupswitch(default_label, case_labels) {
        Util::assert( case_labels.push ); /*FIXME ES4: really "case_labels is Array" */
        Util::assert( case_labels.length > 0 );

        stack(-1);

        if (default_label === undefined) {
            default_label = newLabel();
            for ( var i=0, limit=case_labels.length ; i < limit ; i++ ) {
                Util::assert( case_labels[i] === undefined );
                case_labels[i] = newLabel();
            }
        }

        function map_func(L) { return L.name };
        list3("lookupswitch", default_label.name, Util::map(map_func, case_labels));
        var base = code.length;
        code.uint8(0x1B);
        relativeOffset(base, default_label);
        code.uint30(case_labels.length-1);
        for ( var i=0, limit=case_labels.length ; i < limit ; i++ )
            relativeOffset(base, case_labels[i]);

        return default_label;
    }

    // Standard function calls
    private function call(name, opcode, nargs) {
        stack(1-(nargs+2)); /* pop function/receiver/args; push result */
        list2(name, nargs);
        code.uint8(opcode);
        code.uint30(nargs);
    }

    private function construct(name, opcode, nargs) {
        stack(1-(nargs+1)); /* pop function/receiver/args; push result */
        list2(name, nargs);
        code.uint8(opcode);
        code.uint30(nargs);
    }

    function I_call(nargs) { call("call", 0x41, nargs) }
    function I_construct(nargs) { construct("construct", 0x42, nargs) }

    function I_constructsuper(nargs) {
        stack(nargs+1); /* pop receiver/args */
        list2("constructsuper", nargs);
        code.uint8(0x49);
        code.uint30(nargs);
    }

    private function callIDX(name, opcode, index, nargs) {
        stack(1-(nargs+1)); /* pop receiver/args; push result */
        list3(name, index, nargs);
        code.uint8(opcode);
        code.uint30(index);
        code.uint30(nargs);
    }

    function I_callmethod(index, nargs) { callIDX("callmethod", 0x43, index, nargs) }
    function I_callstatic(index, nargs) { callIDX("callstatic", 0x44, index, nargs) }

    private function callMN(name, opcode, index, nargs, isVoid) {
        /* pop receiver/NS?/Name?/args; push result? */
        var hasRTNS = constants.hasRTNS(index);
        var hasRTName = constants.hasRTName(index);
        stack((isVoid ? 0 : 1) - (1 + (hasRTNS ? 1 : 0) + (hasRTName ? 1 : 0) + nargs));
        list3(name + (hasRTNS ? "<NS>" : "") + (hasRTName ? "<Name>" : ""), index, nargs);
        code.uint8(opcode);
        code.uint30(index);
        code.uint30(nargs);
    }

    function I_callsuper(index, nargs) { callMN("callsuper", 0x45, index, nargs, false) }
    function I_callproperty(index, nargs) { callMN("callproperty", 0x46, index, nargs, false) }
    function I_constructprop(index, nargs) { callMN("constructprop", 0x4A, index, nargs, false) }
    function I_callproplex(index, nargs) { callMN("callproplex", 0x4C, index, nargs, false) }
    function I_callsupervoid(index, nargs) { callMN("callsupervoid", 0x4E, index, nargs, true) }
    function I_callpropvoid(index, nargs) { callMN("callpropvoid", 0x4F, index, nargs, true) }

    function I_debug(debug_type, index, reg, extra=0) {
        //stack(0);
        list5("debug", debug_type, index, reg, extra);
        code.uint8(0xEF);
        code.uint8(debug_type);
        code.uint30(index);
        code.uint8(reg);
        code.uint30(extra);
    }

    /* Generic property operation when there may be a namespace or
       name on the stack.  The instruction pops and pushes some
       fixed amount and may pop one or two more items, depending
       on the kind of name that index references.
    */
    private function propU30(name, pops, pushes, opcode, index) {
        var hasRTNS = constants.hasRTNS(index);
        var hasRTName = constants.hasRTName(index);
        stack(pushes - (pops + (hasRTNS ? 1 : 0) + (hasRTName ? 1 : 0)));
        list2(name + (hasRTNS ? "<NS>" : "") + (hasRTName ? "<Name>" : ""), index);
        code.uint8(opcode);
        code.uint30(index);
    }

    function I_deleteproperty(index) { propU30("deleteproperty", 1, 1, 0x6A, index) }
    function I_getdescendants(index) { propU30("getdescendants", 1, 1, 0x59, index) }
    function I_getproperty(index) { propU30("getproperty", 1, 1, 0x66, index); }
    function I_getsuper(index) { propU30("getsuper", 1, 1, 0x04, index); }
    function I_findproperty(index) { propU30("findproperty", 0, 1, 0x5E, index) }
    function I_findpropstrict(index) { propU30("findpropstrict", 0, 1, 0x5D, index) }
    function I_initproperty(index) { propU30("initproperty", 2, 0, 0x68, index) }
    function I_setproperty(index) { propU30("setproperty", 2, 0, 0x61, index) }
    function I_setsuper(index) { propU30("setsuper", 2, 0, 0x05, index) }

    function I_hasnext2(object_reg, index_reg) {
        stack(1);
        code.uint8(0x32);
        code.uint30(object_reg);
        code.uint30(index_reg);
    }

    function I_newarray(nargs) {
        stack(1 - nargs);
        list2("newarray", nargs);
        code.uint8(0x56);
        code.uint30(nargs);
    }

    function I_newobject(nargs) {
        stack(1 - (2 * nargs));
        list2("newobject", nargs);
        code.uint8(0x55);
        code.uint30(nargs);
    }

    function I_pushbyte(b) {
        stack(1);
        list2("pushbyte", b);
        code.uint8(0x24);
        code.uint8(b);
    }

    function I_setslot(index) {
        stack(-2);
        list2("setslot", index);
        code.uint8(0x6D);
        code.uint30(index);
    }

    function getTemp() {
        if (freeTemps.length > 0)
            return freeTemps.pop();
        else
            return nextTemp++;
    }

    function killTemp(t) {
        freeTemps.push(t);
        I_kill(t);
    }

    function get length() {
        return code.length;
    }

    function finalize() {
        resolveBackpatches();
        return code;
    }

    private function resolveBackpatches() {
        for ( var i=0, limit=backpatches.length ; i < limit ; i++ ) {
            var bp = backpatches[i];
            if (bp.label.address == -1)
                Util::internalError("", 0, "Missing definition for label " + bp.label.name); // FIXME: source pos
            var v = bp.label.address - bp.base;
            code.setInt24(bp.loc, v);
        }
        backpatches.length = 0;
    }

    private function stack(n) {
        current_stack_depth = current_stack_depth + n;
        if (current_stack_depth > max_stack_depth) {
            max_stack_depth = current_stack_depth;
        }
    }

    private function scope(n) {
        current_scope_depth = current_scope_depth + n;
        if (current_scope_depth > max_scope_depth)
            max_scope_depth = current_scope_depth;
    }

}
