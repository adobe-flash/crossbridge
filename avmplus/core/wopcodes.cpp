/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{

#ifdef VMCFG_WORDCODE

#if defined _DEBUG || defined DEBUGGER || defined AVMPLUS_VERBOSE
#  define N(x)  , x
#else
#  define N(x)
#endif

    const WordOpcodeAttr wopAttrs[] =
    {
        // The table is sorted by WordOpcode (see wopcodes.h).
        //
        // The word opcodes will be re-sorted in the near future, please
        // do not depend on ranges that are implicit in this table (eg for
        // the jump opcodes).  Always check the attribute bits.

        // PLEASE NOTE that utils/peephole.as extract information from this
        // table, so keep each row on a separate line, and keep the BEGIN
        // and END lines in place.  Comment lines, blank lines, and
        // trailing '//' comments are OK.  If you add fields you may also
        // need to change the regex that parses this table, in peephole.as,
        // as it knows the order and extracts some of them.
        //
        // DO NOT INSERT #ifdef ... #endif BLOCKS!!

        // width=0 means the instruction is not used in the word-code interpreter; other
        //    attrs should be 0.  Otherwise it's the width in words of the instruction
        //    including the opcode.  Current limit is 7.  Only lookupswitch is variable length
        // jumps=1 means a branch relative to the end of the instruction, offset is 2nd word always
        // term=1  means the instruction terminates control flow (return, throw, lookupswitch)
        // thro=1  means the instruction may throw an exception
        // calls=1 means the topmost value on the stack is an argument count
        // push=n means the instruction pushes n values (after popping).  Current limit is 7
        // pop=n means the instruction pops n values.  Current limit is 7
        // pop_ex=1 means the instruction may pop more values than shown in the pop column,
        //    there is custom logic elsewhere to handle it.  Usually it means there is a
        //    multiname operand
        // use_l=1 means the instruction reads a local slot
        // def_l=1 means the instruction writes a local slot

        // You may assume that the instructions that may invoke user code comprise a subset of
        // the ones that are marked as thro=1.  A few of the latter will not call user code,
        // but at this point we have no formal definition of whether e.g. embedders may
        // or may not subclass MethodEnv in such a way that MethodEnv::findproperty()
        // may call user code.  So there is no separate column for whether such a call may
        // take place.

        // DO NOT ALTER OR MOVE THE NEXT LINE.
        // BEGIN
        // width jumps term thro calls push pop pop_ex use_l def_l       name
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x00") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x01") },
        {    1,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("nop") },             // nop
        {    1,    0,    1,   1,    0,   0,   1,   0,    0,    0         N("throw") },
        {    2,    0,    0,   1,    0,   1,   1,   1,    0,    0         N("getsuper") },
        {    2,    0,    0,   1,    0,   0,   2,   1,    0,    0         N("setsuper") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    0,    0         N("dxns") },
        {    1,    0,    0,   1,    0,   0,   1,   0,    0,    0         N("dxnslate") },
        {    2,    0,    0,   0,    0,   0,   0,   0,    1,    1         N("kill") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x09") },            // label
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x0A") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x0B") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifnlt") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifnle") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifngt") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifnge") },
        {    2,    1,    1,   0,    0,   0,   0,   0,    0,    0         N("jump") },            // 0x10
        {    2,    1,    0,   0,    0,   0,   1,   0,    0,    0         N("iftrue") },
        {    2,    1,    0,   0,    0,   0,   1,   0,    0,    0         N("iffalse") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifeq") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifne") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("iflt") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifle") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifgt") },
        {    2,    1,    0,   1,    0,   0,   2,   0,    0,    0         N("ifge") },
        {    2,    1,    0,   0,    0,   0,   2,   0,    0,    0         N("ifstricteq") },
        {    2,    1,    0,   0,    0,   0,   2,   0,    0,    0         N("ifstrictne") },
        {    3,    1,    1,   0,    0,   0,   1,   0,    0,    0         N("lookupswitch") },    // NOTE, variable length beyond 3rd word
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    0         N("pushwith") },
        {    1,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("popscope") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("nextname") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("hasnext") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushnull") },        // 0x20
        {    1,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushundefined") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushfloat") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("nextvalue") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x24") },            // pushbyte
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x25") },            // pushshort
        {    1,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushtrue") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushfalse") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushnan") },
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    0         N("pop") },
        {    1,    0,    0,   0,    0,   2,   1,   0,    0,    0         N("dup") },
        {    1,    0,    0,   0,    0,   2,   2,   0,    0,    0         N("swap") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushstring") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x2D") },            // pushint
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x2E") },            // pushuint
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushdouble") },
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    0         N("pushscope") },       // 0x30
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushnamespace") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("hasnext2") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x33") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x34") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("li8") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("li16") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("li32") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("lf32") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("lf64") },
        {    1,    0,    0,   1,    0,   0,   2,   0,    0,    0         N("si8") },
        {    1,    0,    0,   1,    0,   0,   2,   0,    0,    0         N("si16") },
        {    1,    0,    0,   1,    0,   0,   2,   0,    0,    0         N("si32") },
        {    1,    0,    0,   1,    0,   0,   2,   0,    0,    0         N("sf32") },
        {    1,    0,    0,   1,    0,   0,   2,   0,    0,    0         N("sf64") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x3F") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("newfunction") },     // 0x40
        {    2,    0,    0,   1,    1,   1,   2,   0,    0,    0         N("call") },
        {    2,    0,    0,   1,    1,   1,   1,   0,    0,    0         N("construct") },
        {    3,    0,    0,   1,    1,   1,   1,   0,    0,    0         N("callmethod") },
        {    3,    0,    0,   1,    1,   1,   1,   0,    0,    0         N("callstatic") },
        {    3,    0,    0,   1,    1,   1,   1,   1,    0,    0         N("callsuper") },
        {    3,    0,    0,   1,    1,   1,   1,   1,    0,    0         N("callproperty") },
        {    1,    0,    1,   0,    0,   0,   0,   0,    0,    0         N("returnvoid") },
        {    1,    0,    1,   0,    0,   0,   1,   0,    0,    0         N("returnvalue") },
        {    2,    0,    0,   1,    1,   0,   1,   0,    0,    0         N("constructsuper") },
        {    3,    0,    0,   1,    1,   1,   1,   1,    0,    0         N("constructprop") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x4B") },            // callsuperid
        {    3,    0,    0,   1,    1,   1,   1,   1,    0,    0         N("callproplex") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x4D") },            // callinterface
        {    3,    0,    0,   1,    1,   0,   1,   1,    0,    0         N("callsupervoid") },
        {    3,    0,    0,   1,    1,   0,   1,   1,    0,    0         N("callpropvoid") },
        {    1,    0,    0,   0,    0,   1,   1,   0,    0,    0         N("sxi1") },
        {    1,    0,    0,   0,    0,   1,   1,   0,    0,    0         N("sxi8") },
        {    1,    0,    0,   0,    0,   1,   1,   0,    0,    0         N("sxi16") },
        {    2,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("applytype") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushfloat4") },
        {    2,    0,    0,   1,    1,   1,   0,   0,    0,    0         N("newobject") },
        {    2,    0,    0,   1,    1,   1,   0,   0,    0,    0         N("newarray") },
        {    1,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("newactivation") },
        {    2,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("newclass") },
        {    2,    0,    0,   1,    0,   1,   1,   1,    0,    0         N("getdescendants") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("newcatch") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x5B") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x5C") },
        {    2,    0,    0,   1,    0,   1,   0,   1,    0,    0         N("findpropstrict") },
        {    2,    0,    0,   1,    0,   1,   0,   1,    0,    0         N("findproperty") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("finddef") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("getlex") },          // 0x60
        {    2,    0,    0,   1,    0,   0,   2,   1,    0,    0         N("setproperty") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    1,    0         N("getlocal") },
        {    2,    0,    0,   0,    0,   0,   1,   0,    0,    1         N("setlocal") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("getglobalscope") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("getscopeobject") },
        {    2,    0,    0,   1,    0,   1,   1,   1,    0,    0         N("getproperty") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("getouterscope") },
        {    2,    0,    0,   1,    0,   0,   2,   1,    0,    0         N("initproperty") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x69") },
        {    2,    0,    0,   1,    0,   1,   1,   1,    0,    0         N("deleteproperty") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x6B") },
        {    2,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("getslot") },
        {    2,    0,    0,   1,    0,   0,   2,   0,    0,    0         N("setslot") },
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("getglobalslot") },
        {    2,    0,    0,   0,    0,   0,   1,   0,    0,    0         N("setglobalslot") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_s") },       // 0x70
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("esc_xelem") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("esc_xattr") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_i") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_u") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_d") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_b") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_o") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("checkfilter") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_f") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("unplus") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("convert_f4") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x7C") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x7D") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x7E") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x7F") },
        {    2,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("coerce") },          // 0x80
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x81") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x82") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x83") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x84") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("coerce_s") },
        {    2,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("astype") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("astypelate") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x88") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("coerce_o") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x8A") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x8B") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x8C") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x8D") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x8E") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x8F") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("negate") },          // 0x90
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("increment") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    1,    1         N("inclocal") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("decrement") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    1,    1         N("declocal") },
        {    1,    0,    0,   0,    0,   1,   1,   0,    0,    0         N("typeof") },
        {    1,    0,    0,   0,    0,   1,   1,   0,    0,    0         N("not") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("bitnot") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x98") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x99") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x9A") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x9B") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x9C") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x9D") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x9E") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x9F") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("add") },             // 0xA0
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("subtract") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("multiply") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("divide") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("modulo") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("lshift") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("rshift") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("urshift") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("bitand") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("bitor") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("bitxor") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("equals") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("strictequals") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("lessthan") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("lessequals") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("greaterthan") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("greaterequals") },   // 0xB0
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("instanceof") },
        {    2,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("istype") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("istypelate") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("in") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xB5") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xB6") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xB7") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xB8") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xB9") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xBA") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xBB") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xBC") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xBD") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xBE") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xBF") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("increment_i") },     // 0xC0
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("decrement_i") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    1,    1         N("inclocal_i") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    1,    1         N("declocal_i") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("negate_i") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("add_i") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("subtract_i") },
        {    1,    0,    0,   1,    0,   1,   2,   0,    0,    0         N("multiply_i") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xC8") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xC9") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xCA") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xCB") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xCC") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xCD") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xCE") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xCF") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    1,    0         N("getlocal0") },       // 0xD0
        {    1,    0,    0,   0,    0,   1,   0,   0,    1,    0         N("getlocal1") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    1,    0         N("getlocal2") },
        {    1,    0,    0,   0,    0,   1,   0,   0,    1,    0         N("getlocal3") },
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    1         N("setlocal0") },
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    1         N("setlocal1") },
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    1         N("setlocal2") },
        {    1,    0,    0,   0,    0,   0,   1,   0,    0,    1         N("setlocal3") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xD8") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xD9") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xDA") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xDB") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xDC") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xDD") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xDE") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xDF") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE0") },            // 0xE0
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE1") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE2") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE3") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE4") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE5") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE6") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE7") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE8") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xE9") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xEA") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xEB") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xEC") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xED") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xEE") },            // formerly abs_jump
        {    5,    0,    0,   1,    0,   0,   0,   0,    0,    0         N("debug") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    0,    0         N("debugline") },       // 0xF0
        {    2,    0,    0,   1,    0,   0,   0,   0,    0,    0         N("debugfile") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF2") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF3") },            // timestamp
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF4") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF5") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF6") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF7") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF8") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xF9") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xFA") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xFB") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xFC") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xFD") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xFE") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0xFF") },
        {    0,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("0x100") },           // 0x100
        {    2,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("pushbits") },
        {    3,    0,    0,   0,    0,   1,   0,   0,    0,    0         N("push_doublebits") },
        {    2,    0,    0,   0,    0,   2,   0,   0,    1,    0         N("get2locals") },
        {    2,    0,    0,   0,    0,   3,   0,   0,    1,    0         N("get3locals") },
        {    2,    0,    0,   0,    0,   4,   0,   0,    1,    0         N("get4locals") },
        {    2,    0,    0,   0,    0,   5,   0,   0,    1,    0         N("get5locals") },
        {    2,    0,    0,   0,    0,   0,   0,   0,    0,    1         N("storelocal") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("add_ll") },
        {    2,    0,    0,   1,    0,   0,   0,   0,    1,    1         N("add_set_lll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("subtract_ll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("multiply_ll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("divide_ll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("modulo_ll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("bitand_ll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("bitor_ll") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("bitxor_ll") },       // 0x110
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("add_lb") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("subtract_lb") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("multiply_lb") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("divide_lb") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("bitand_lb") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("bitor_lb") },
        {    3,    0,    0,   1,    0,   1,   0,   0,    1,    0         N("bitxor_lb") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("iflt_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifnlt_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifle_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifnle_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifgt_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifngt_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifge_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifnge_ll") },
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifeq_ll") },         // 0x120
        {    3,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifne_ll") },
        {    3,    1,    0,   0,    0,   0,   0,   0,    1,    0         N("ifstricteq_ll") },
        {    3,    1,    0,   0,    0,   0,   0,   0,    1,    0         N("ifstrictne_ll") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("iflt_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifnlt_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifle_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifnle_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifgt_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifngt_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifge_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifnge_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifeq_lb") },
        {    4,    1,    0,   1,    0,   0,   0,   0,    1,    0         N("ifne_lb") },
        {    4,    1,    0,   0,    0,   0,   0,   0,    1,    0         N("ifstricteq_lb") },
        {    4,    1,    0,   0,    0,   0,   0,   0,    1,    0         N("ifstrictne_lb") },
        {    1,    0,    0,   0,    0,   1,   2,   0,    0,    0         N("swap_pop") },        // 0x130
        {    2,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("findpropglobal") },
        {    2,    0,    0,   1,    0,   1,   0,   0,    0,    0         N("findpropglobalstrict") },
        {    1,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("debugenter") },
        {    1,    0,    0,   0,    0,   0,   0,   0,    0,    0         N("debugexit") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("lix8") },
        {    1,    0,    0,   1,    0,   1,   1,   0,    0,    0         N("lix16") },
        {    1,    0,    0,   1,    0,   0,   0,   0,    0,    0         N("float4") },
        // DO NOT ALTER OR MOVE THE NEXT LINE.
        // END
    };

#endif // VMCFG_WORDCODE

}
