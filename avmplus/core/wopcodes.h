/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

#ifdef VMCFG_WORDCODE

    // Try to keep the opcodes commented out if the table in Interpreter.cpp does
    // not use them; it's the best way of catching errors elsewhere.

    enum WordOpcode
    {
        // These are unused but the table generation in ActionBlockConstants.cpp
        // and the enum generation in ActionBlockConstants.h requires them to be
        // known.  We map them to 0.
        WOP_bkpt = 0,
        WOP_label = 0,
        WOP_pushbyte = 0,
        WOP_pushshort = 0,
        WOP_pushint = 0,
        WOP_pushuint = 0,
        WOP_callsuperid = 0,
        WOP_callinterface = 0,
        WOP_coerce_a = 0,
        WOP_bkptline = 0,
        WOP_timestamp = 0,
        WOP_restargc = 0,
        WOP_restarg = 0,

        // Normal operations
        WOP_nop = 0x02,
        WOP_throw = 0x03,
        WOP_getsuper = 0x04,
        WOP_setsuper = 0x05,
        WOP_dxns = 0x06,
        WOP_dxnslate = 0x07,
        WOP_kill = 0x08,
        WOP_lf32x4 = 0x0A,
        WOP_sf32x4 = 0x0B,
        WOP_ifnlt = 0x0C,
        WOP_ifnle = 0x0D,
        WOP_ifngt = 0x0E,
        WOP_ifnge = 0x0F,
        WOP_jump = 0x10,
        WOP_iftrue = 0x11,
        WOP_iffalse = 0x12,
        WOP_ifeq = 0x13,
        WOP_ifne = 0x14,
        WOP_iflt = 0x15,
        WOP_ifle = 0x16,
        WOP_ifgt = 0x17,
        WOP_ifge = 0x18,
        WOP_ifstricteq = 0x19,
        WOP_ifstrictne = 0x1A,
        WOP_lookupswitch = 0x1B,
        WOP_pushwith = 0x1C,
        WOP_popscope = 0x1D,
        WOP_nextname = 0x1E,
        WOP_hasnext = 0x1F,
        WOP_pushnull = 0x20,
        WOP_pushundefined = 0x21,
        WOP_pushfloat = 0x22,
        WOP_nextvalue = 0x23,
        WOP_pushtrue = 0x26,
        WOP_pushfalse = 0x27,
        WOP_pushnan = 0x28,
        WOP_pop = 0x29,
        WOP_dup = 0x2A,
        WOP_swap = 0x2B,
        WOP_pushstring = 0x2C,
        WOP_pushdouble = 0x2F,
        WOP_pushscope = 0x30,
        WOP_pushnamespace = 0x31,
        WOP_hasnext2 = 0x32,
        WOP_li8 = 0x35,
        WOP_li16 = 0x36,
        WOP_li32 = 0x37,
        WOP_lf32 = 0x38,
        WOP_lf64 = 0x39,
        WOP_si8 = 0x3A,
        WOP_si16 = 0x3B,
        WOP_si32 = 0x3C,
        WOP_sf32 = 0x3D,
        WOP_sf64 = 0x3E,
        WOP_newfunction = 0x40,
        WOP_call = 0x41,
        WOP_construct = 0x42,
        WOP_callmethod = 0x43,
        WOP_callstatic = 0x44,
        WOP_callsuper = 0x45,
        WOP_callproperty = 0x46,
        WOP_returnvoid = 0x47,
        WOP_returnvalue = 0x48,
        WOP_constructsuper = 0x49,
        WOP_constructprop = 0x4A,
        WOP_callproplex = 0x4C,
        WOP_callsupervoid = 0x4E,
        WOP_callpropvoid = 0x4F,
        WOP_sxi1 = 0x50,
        WOP_sxi8 = 0x51,
        WOP_sxi16 = 0x52,
        WOP_applytype = 0x53,
        WOP_pushfloat4 = 0x54,
        WOP_newobject = 0x55,
        WOP_newarray = 0x56,
        WOP_newactivation = 0x57,
        WOP_newclass = 0x58,
        WOP_getdescendants = 0x59,
        WOP_newcatch = 0x5A,
        WOP_findpropstrict = 0x5D,
        WOP_findproperty = 0x5E,
        WOP_finddef = 0x5F,
        WOP_getlex = 0x60,
        WOP_setproperty = 0x61,
        WOP_getlocal = 0x62,
        WOP_setlocal = 0x63,
        WOP_getglobalscope = 0x64,
        WOP_getscopeobject = 0x65,
        WOP_getproperty = 0x66,
        WOP_getouterscope = 0x67,
        WOP_initproperty = 0x68,
        WOP_deleteproperty = 0x6A,
        WOP_getslot = 0x6C,
        WOP_setslot = 0x6D,
        WOP_getglobalslot = 0x6E,
        WOP_setglobalslot = 0x6F,
        WOP_convert_s = 0x70,
        WOP_esc_xelem = 0x71,
        WOP_esc_xattr = 0x72,
        WOP_convert_i = 0x73,
        WOP_coerce_i = 0x73, // coerce_i -> convert_i, they are the same
        WOP_convert_u = 0x74,
        WOP_coerce_u = 0x64, // coerce_u -> convert_u, they are the same
        WOP_convert_d = 0x75,
        WOP_coerce_d = 0x75, // coerce_d -> convert_d, they are the same
        WOP_convert_b = 0x76,
        WOP_coerce_b = 0x76, // coerce_b -> convert_b, they are the same
        WOP_convert_o = 0x77,
        WOP_checkfilter = 0x78,
        WOP_convert_f = 0x79,
        WOP_unplus = 0x7A,
        WOP_convert_f4 = 0x7B,
        WOP_coerce = 0x80,
        WOP_coerce_s = 0x85,
        WOP_astype = 0x86,
        WOP_astypelate = 0x87,
        WOP_coerce_o = 0x89,
        WOP_negate = 0x90,
        WOP_increment = 0x91,
        WOP_inclocal = 0x92,
        WOP_decrement = 0x93,
        WOP_declocal = 0x94,
        WOP_typeof = 0x95,
        WOP_not = 0x96,
        WOP_bitnot = 0x97,
        WOP_add = 0xA0,
        WOP_subtract = 0xA1,
        WOP_multiply = 0xA2,
        WOP_divide = 0xA3,
        WOP_modulo = 0xA4,
        WOP_lshift = 0xA5,
        WOP_rshift = 0xA6,
        WOP_urshift = 0xA7,
        WOP_bitand = 0xA8,
        WOP_bitor = 0xA9,
        WOP_bitxor = 0xAA,
        WOP_equals = 0xAB,
        WOP_strictequals = 0xAC,
        WOP_lessthan = 0xAD,
        WOP_lessequals = 0xAE,
        WOP_greaterthan = 0xAF,
        WOP_greaterequals = 0xB0,
        WOP_instanceof = 0xB1,
        WOP_istype = 0xB2,
        WOP_istypelate = 0xB3,
        WOP_in = 0xB4,
        WOP_increment_i = 0xC0,
        WOP_decrement_i = 0xC1,
        WOP_inclocal_i = 0xC2,
        WOP_declocal_i = 0xC3,
        WOP_negate_i = 0xC4,
        WOP_add_i = 0xC5,
        WOP_subtract_i = 0xC6,
        WOP_multiply_i = 0xC7,
        WOP_getlocal0 = 0xD0,
        WOP_getlocal1 = 0xD1,
        WOP_getlocal2 = 0xD2,
        WOP_getlocal3 = 0xD3,
        WOP_setlocal0 = 0xD4,
        WOP_setlocal1 = 0xD5,
        WOP_setlocal2 = 0xD6,
        WOP_setlocal3 = 0xD7,
        WOP_debug = 0xEF,
        WOP_debugline = 0xF0,
        WOP_debugfile = 0xF1,
        WOP_pushbits = 0x101,
        WOP_push_doublebits = 0x102,
        // begin VMCFG_WORDCODE_PEEPHOLE
        WOP_get2locals = 0x103,
        WOP_get3locals = 0x104,
        WOP_get4locals = 0x105,
        WOP_get5locals = 0x106,
        WOP_storelocal = 0x107,
        WOP_add_ll = 0x108,
        WOP_add_set_lll = 0x109,
        WOP_subtract_ll = 0x10A,
        WOP_multiply_ll = 0x10B,
        WOP_divide_ll = 0x10C,
        WOP_modulo_ll = 0x10D,
        WOP_bitand_ll = 0x10E,
        WOP_bitor_ll = 0x10F,
        WOP_bitxor_ll = 0x110,
        WOP_add_lb = 0x111,
        WOP_subtract_lb = 0x112,
        WOP_multiply_lb = 0x113,
        WOP_divide_lb = 0x114,
        WOP_bitand_lb = 0x115,
        WOP_bitor_lb = 0x116,
        WOP_bitxor_lb = 0x117,
        WOP_iflt_ll = 0x118,
        WOP_ifnlt_ll = 0x119,
        WOP_ifle_ll = 0x11A,
        WOP_ifnle_ll = 0x11B,
        WOP_ifgt_ll = 0x11C,
        WOP_ifngt_ll = 0x11D,
        WOP_ifge_ll = 0x11E,
        WOP_ifnge_ll = 0x11F,
        WOP_ifeq_ll = 0x120,
        WOP_ifne_ll = 0x121,
        WOP_ifstricteq_ll = 0x122,
        WOP_ifstrictne_ll = 0x123,
        WOP_iflt_lb = 0x124,
        WOP_ifnlt_lb = 0x125,
        WOP_ifle_lb = 0x126,
        WOP_ifnle_lb = 0x127,
        WOP_ifgt_lb = 0x128,
        WOP_ifngt_lb = 0x129,
        WOP_ifge_lb = 0x12A,
        WOP_ifnge_lb = 0x12B,
        WOP_ifeq_lb = 0x12C,
        WOP_ifne_lb = 0x12D,
        WOP_ifstricteq_lb = 0x12E,
        WOP_ifstrictne_lb = 0x12F,
        WOP_swap_pop = 0x130,
        // end VMCFG_WORDCODE_PEEPHOLE
        WOP_findpropglobal = 0x131,
        WOP_findpropglobalstrict = 0x132,
        WOP_debugenter = 0x133,
        WOP_debugexit = 0x134,
        WOP_lix8 = 0x135,
        WOP_lix16 = 0x136,
        WOP_float4 = 0x137,     // Needs four arguments but does not pop any - an abomination brought on by LIR_ffff2f4 and writeNip()

        WOP_LAST = 0x137
    };

    struct WordOpcodeAttr
    {
        unsigned width:3;        // Number of words including opcode
        unsigned jumps:1;        // True if the instruction jumps
        unsigned terminates:1;   // True if the instruction terminates control flow (return, throw)
        unsigned throws:1;       // True if the instruction may throw an exception (not including timer interrupts and other unhandleable errors)
        unsigned calls:1;        // True if the last argument is arg_count
                                 // An 'arg_count' number of extra parameters are pop from the stack
        unsigned pushes:3;       // Number of items pushed on the stack
        unsigned pops:3;         // Number of items popped from the stack
        unsigned pops_extra:1;   // True if the instruction has extra stack pops, determined by verifier
        unsigned uses_local:1;   // Use local slot
        unsigned defs_local:1;   // Defines local slot

#if defined _DEBUG || defined DEBUGGER || defined AVMPLUS_VERBOSE
        // Keep this field last
        const char * name;       // Printable name for the instruction
#endif
    };

    extern const WordOpcodeAttr wopAttrs[];

#endif // VMCFG_WORDCODE

}
