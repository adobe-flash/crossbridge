/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//////////////////////////////////////////////////////////////////////////////////
// GENERATED FILE: DO NOT EDIT!!! See utils/opcodes.as in the tamarin repository.
//////////////////////////////////////////////////////////////////////////////////

// This file is to be included by its clients.

const OP_bkpt : int = 0x01;
const OP_nop : int = 0x02;
const OP_throw : int = 0x03;
const OP_getsuper : int = 0x04;
const OP_setsuper : int = 0x05;
const OP_dxns : int = 0x06;
const OP_dxnslate : int = 0x07;
const OP_kill : int = 0x08;
const OP_label : int = 0x09;
const OP_lf32x4 : int = 0x0a;
const OP_sf32x4 : int = 0x0b;
const OP_ifnlt : int = 0x0c;
const OP_ifnle : int = 0x0d;
const OP_ifngt : int = 0x0e;
const OP_ifnge : int = 0x0f;
const OP_jump : int = 0x10;
const OP_iftrue : int = 0x11;
const OP_iffalse : int = 0x12;
const OP_ifeq : int = 0x13;
const OP_ifne : int = 0x14;
const OP_iflt : int = 0x15;
const OP_ifle : int = 0x16;
const OP_ifgt : int = 0x17;
const OP_ifge : int = 0x18;
const OP_ifstricteq : int = 0x19;
const OP_ifstrictne : int = 0x1a;
const OP_lookupswitch : int = 0x1b;
const OP_pushwith : int = 0x1c;
const OP_popscope : int = 0x1d;
const OP_nextname : int = 0x1e;
const OP_hasnext : int = 0x1f;
const OP_pushnull : int = 0x20;
const OP_pushundefined : int = 0x21;
const OP_pushfloat : int = 0x22;
const OP_nextvalue : int = 0x23;
const OP_pushbyte : int = 0x24;
const OP_pushshort : int = 0x25;
const OP_pushtrue : int = 0x26;
const OP_pushfalse : int = 0x27;
const OP_pushnan : int = 0x28;
const OP_pop : int = 0x29;
const OP_dup : int = 0x2a;
const OP_swap : int = 0x2b;
const OP_pushstring : int = 0x2c;
const OP_pushint : int = 0x2d;
const OP_pushuint : int = 0x2e;
const OP_pushdouble : int = 0x2f;
const OP_pushscope : int = 0x30;
const OP_pushnamespace : int = 0x31;
const OP_hasnext2 : int = 0x32;
const OP_li8 : int = 0x35;
const OP_li16 : int = 0x36;
const OP_li32 : int = 0x37;
const OP_lf32 : int = 0x38;
const OP_lf64 : int = 0x39;
const OP_si8 : int = 0x3a;
const OP_si16 : int = 0x3b;
const OP_si32 : int = 0x3c;
const OP_sf32 : int = 0x3d;
const OP_sf64 : int = 0x3e;
const OP_newfunction : int = 0x40;
const OP_call : int = 0x41;
const OP_construct : int = 0x42;
const OP_callmethod : int = 0x43;
const OP_callstatic : int = 0x44;
const OP_callsuper : int = 0x45;
const OP_callproperty : int = 0x46;
const OP_returnvoid : int = 0x47;
const OP_returnvalue : int = 0x48;
const OP_constructsuper : int = 0x49;
const OP_constructprop : int = 0x4a;
const OP_callproplex : int = 0x4c;
const OP_callsupervoid : int = 0x4e;
const OP_callpropvoid : int = 0x4f;
const OP_sxi1 : int = 0x50;
const OP_sxi8 : int = 0x51;
const OP_sxi16 : int = 0x52;
const OP_applytype : int = 0x53;
const OP_pushfloat4 : int = 0x54;
const OP_newobject : int = 0x55;
const OP_newarray : int = 0x56;
const OP_newactivation : int = 0x57;
const OP_newclass : int = 0x58;
const OP_getdescendants : int = 0x59;
const OP_newcatch : int = 0x5a;
const OP_findpropstrict : int = 0x5d;
const OP_findproperty : int = 0x5e;
const OP_finddef : int = 0x5f;
const OP_getlex : int = 0x60;
const OP_setproperty : int = 0x61;
const OP_getlocal : int = 0x62;
const OP_setlocal : int = 0x63;
const OP_getglobalscope : int = 0x64;
const OP_getscopeobject : int = 0x65;
const OP_getproperty : int = 0x66;
const OP_getouterscope : int = 0x67;
const OP_initproperty : int = 0x68;
const OP_deleteproperty : int = 0x6a;
const OP_getslot : int = 0x6c;
const OP_setslot : int = 0x6d;
const OP_getglobalslot : int = 0x6e;
const OP_setglobalslot : int = 0x6f;
const OP_convert_s : int = 0x70;
const OP_esc_xelem : int = 0x71;
const OP_esc_xattr : int = 0x72;
const OP_convert_i : int = 0x73;
const OP_convert_u : int = 0x74;
const OP_convert_d : int = 0x75;
const OP_convert_b : int = 0x76;
const OP_convert_o : int = 0x77;
const OP_checkfilter : int = 0x78;
const OP_convert_f : int = 0x79;
const OP_unplus : int = 0x7a;
const OP_convert_f4 : int = 0x7b;
const OP_coerce : int = 0x80;
const OP_coerce_b : int = 0x81;
const OP_coerce_a : int = 0x82;
const OP_coerce_i : int = 0x83;
const OP_coerce_d : int = 0x84;
const OP_coerce_s : int = 0x85;
const OP_astype : int = 0x86;
const OP_astypelate : int = 0x87;
const OP_coerce_u : int = 0x88;
const OP_coerce_o : int = 0x89;
const OP_negate : int = 0x90;
const OP_increment : int = 0x91;
const OP_inclocal : int = 0x92;
const OP_decrement : int = 0x93;
const OP_declocal : int = 0x94;
const OP_typeof : int = 0x95;
const OP_not : int = 0x96;
const OP_bitnot : int = 0x97;
const OP_add : int = 0xa0;
const OP_subtract : int = 0xa1;
const OP_multiply : int = 0xa2;
const OP_divide : int = 0xa3;
const OP_modulo : int = 0xa4;
const OP_lshift : int = 0xa5;
const OP_rshift : int = 0xa6;
const OP_urshift : int = 0xa7;
const OP_bitand : int = 0xa8;
const OP_bitor : int = 0xa9;
const OP_bitxor : int = 0xaa;
const OP_equals : int = 0xab;
const OP_strictequals : int = 0xac;
const OP_lessthan : int = 0xad;
const OP_lessequals : int = 0xae;
const OP_greaterthan : int = 0xaf;
const OP_greaterequals : int = 0xb0;
const OP_instanceof : int = 0xb1;
const OP_istype : int = 0xb2;
const OP_istypelate : int = 0xb3;
const OP_in : int = 0xb4;
const OP_increment_i : int = 0xc0;
const OP_decrement_i : int = 0xc1;
const OP_inclocal_i : int = 0xc2;
const OP_declocal_i : int = 0xc3;
const OP_negate_i : int = 0xc4;
const OP_add_i : int = 0xc5;
const OP_subtract_i : int = 0xc6;
const OP_multiply_i : int = 0xc7;
const OP_getlocal0 : int = 0xd0;
const OP_getlocal1 : int = 0xd1;
const OP_getlocal2 : int = 0xd2;
const OP_getlocal3 : int = 0xd3;
const OP_setlocal0 : int = 0xd4;
const OP_setlocal1 : int = 0xd5;
const OP_setlocal2 : int = 0xd6;
const OP_setlocal3 : int = 0xd7;
const OP_debug : int = 0xef;
const OP_debugline : int = 0xf0;
const OP_debugfile : int = 0xf1;
const OP_bkptline : int = 0xf2;
const OP_timestamp : int = 0xf3;
const opNames : Array = [
    "OP_0x00       ",
    "bkpt          ",
    "nop           ",
    "throw         ",
    "getsuper      ",
    "setsuper      ",
    "dxns          ",
    "dxnslate      ",
    "kill          ",
    "label         ",
    "lf32x4        ",
    "sf32x4        ",
    "ifnlt         ",
    "ifnle         ",
    "ifngt         ",
    "ifnge         ",
    "jump          ",
    "iftrue        ",
    "iffalse       ",
    "ifeq          ",
    "ifne          ",
    "iflt          ",
    "ifle          ",
    "ifgt          ",
    "ifge          ",
    "ifstricteq    ",
    "ifstrictne    ",
    "lookupswitch  ",
    "pushwith      ",
    "popscope      ",
    "nextname      ",
    "hasnext       ",
    "pushnull      ",
    "pushundefined ",
    "pushfloat     ",
    "nextvalue     ",
    "pushbyte      ",
    "pushshort     ",
    "pushtrue      ",
    "pushfalse     ",
    "pushnan       ",
    "pop           ",
    "dup           ",
    "swap          ",
    "pushstring    ",
    "pushint       ",
    "pushuint      ",
    "pushdouble    ",
    "pushscope     ",
    "pushnamespace ",
    "hasnext2      ",
    "OP_0x33       ",
    "OP_0x34       ",
    "li8           ",
    "li16          ",
    "li32          ",
    "lf32          ",
    "lf64          ",
    "si8           ",
    "si16          ",
    "si32          ",
    "sf32          ",
    "sf64          ",
    "OP_0x3F       ",
    "newfunction   ",
    "call          ",
    "construct     ",
    "callmethod    ",
    "callstatic    ",
    "callsuper     ",
    "callproperty  ",
    "returnvoid    ",
    "returnvalue   ",
    "constructsuper",
    "constructprop ",
    "OP_0x4B       ",
    "callproplex   ",
    "OP_0x4D       ",
    "callsupervoid ",
    "callpropvoid  ",
    "sxi1          ",
    "sxi8          ",
    "sxi16         ",
    "applytype     ",
    "pushfloat4    ",
    "newobject     ",
    "newarray      ",
    "newactivation ",
    "newclass      ",
    "getdescendants",
    "newcatch      ",
    "OP_0x5B       ",
    "OP_0x5C       ",
    "findpropstrict",
    "findproperty  ",
    "finddef       ",
    "getlex        ",
    "setproperty   ",
    "getlocal      ",
    "setlocal      ",
    "getglobalscope",
    "getscopeobject",
    "getproperty   ",
    "getouterscope ",
    "initproperty  ",
    "OP_0x69       ",
    "deleteproperty",
    "OP_0x6B       ",
    "getslot       ",
    "setslot       ",
    "getglobalslot ",
    "setglobalslot ",
    "convert_s     ",
    "esc_xelem     ",
    "esc_xattr     ",
    "convert_i     ",
    "convert_u     ",
    "convert_d     ",
    "convert_b     ",
    "convert_o     ",
    "checkfilter   ",
    "convert_f     ",
    "unplus        ",
    "convert_f4    ",
    "OP_0x7C       ",
    "OP_0x7D       ",
    "OP_0x7E       ",
    "OP_0x7F       ",
    "coerce        ",
    "coerce_b      ",
    "coerce_a      ",
    "coerce_i      ",
    "coerce_d      ",
    "coerce_s      ",
    "astype        ",
    "astypelate    ",
    "coerce_u      ",
    "coerce_o      ",
    "OP_0x8A       ",
    "OP_0x8B       ",
    "OP_0x8C       ",
    "OP_0x8D       ",
    "OP_0x8E       ",
    "OP_0x8F       ",
    "negate        ",
    "increment     ",
    "inclocal      ",
    "decrement     ",
    "declocal      ",
    "typeof        ",
    "not           ",
    "bitnot        ",
    "OP_0x98       ",
    "OP_0x99       ",
    "OP_0x9A       ",
    "OP_0x9B       ",
    "OP_0x9C       ",
    "OP_0x9D       ",
    "OP_0x9E       ",
    "OP_0x9F       ",
    "add           ",
    "subtract      ",
    "multiply      ",
    "divide        ",
    "modulo        ",
    "lshift        ",
    "rshift        ",
    "urshift       ",
    "bitand        ",
    "bitor         ",
    "bitxor        ",
    "equals        ",
    "strictequals  ",
    "lessthan      ",
    "lessequals    ",
    "greaterthan   ",
    "greaterequals ",
    "instanceof    ",
    "istype        ",
    "istypelate    ",
    "in            ",
    "OP_0xB5       ",
    "OP_0xB6       ",
    "OP_0xB7       ",
    "OP_0xB8       ",
    "OP_0xB9       ",
    "OP_0xBA       ",
    "OP_0xBB       ",
    "OP_0xBC       ",
    "OP_0xBD       ",
    "OP_0xBE       ",
    "OP_0xBF       ",
    "increment_i   ",
    "decrement_i   ",
    "inclocal_i    ",
    "declocal_i    ",
    "negate_i      ",
    "add_i         ",
    "subtract_i    ",
    "multiply_i    ",
    "OP_0xC8       ",
    "OP_0xC9       ",
    "OP_0xCA       ",
    "OP_0xCB       ",
    "OP_0xCC       ",
    "OP_0xCD       ",
    "OP_0xCE       ",
    "OP_0xCF       ",
    "getlocal0     ",
    "getlocal1     ",
    "getlocal2     ",
    "getlocal3     ",
    "setlocal0     ",
    "setlocal1     ",
    "setlocal2     ",
    "setlocal3     ",
    "OP_0xD8       ",
    "OP_0xD9       ",
    "OP_0xDA       ",
    "OP_0xDB       ",
    "OP_0xDC       ",
    "OP_0xDD       ",
    "OP_0xDE       ",
    "OP_0xDF       ",
    "OP_0xE0       ",
    "OP_0xE1       ",
    "OP_0xE2       ",
    "OP_0xE3       ",
    "OP_0xE4       ",
    "OP_0xE5       ",
    "OP_0xE6       ",
    "OP_0xE7       ",
    "OP_0xE8       ",
    "OP_0xE9       ",
    "OP_0xEA       ",
    "OP_0xEB       ",
    "OP_0xEC       ",
    "OP_0xED       ",
    "OP_0xEE       ",
    "debug         ",
    "debugline     ",
    "debugfile     ",
    "bkptline      ",
    "timestamp     ",
    "OP_0xF4       ",
    "OP_0xF5       ",
    "OP_0xF6       ",
    "OP_0xF7       ",
    "OP_0xF8       ",
    "OP_0xF9       ",
    "OP_0xFA       ",
    "OP_0xFB       ",
    "OP_0xFC       ",
    "OP_0xFD       ",
    "OP_0xFE       ",
    "OP_0xFF       ",
];
