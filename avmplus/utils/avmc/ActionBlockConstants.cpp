/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// NOTE.  This file has been copied from ../../core/ActionBlockConstants.cpp.
// It has been copied because it's not been practical to get Xcode not to pick
// up core/avmplus.h on the line below.  Because of the PCH setup on Windows,
// the #include cannot be conditional.

#include "avmplus.h"

namespace avmplus
{
    namespace ActionBlockConstants
    {
        const unsigned char kindToPushOp[] = {
            0,
            OP_pushstring, // CONSTANT_Utf8=1
            0,
            OP_pushint,  // CONSTANT_Int=3
            OP_pushuint,  // CONSTANT_UInt=4
            OP_pushnamespace, // CONSTANT_PrivateNs=5
            OP_pushdouble, // CONSTANT_Double=6
            0,
            OP_pushnamespace, // CONSTANT_Namespace=8
            0,
            OP_pushfalse, // CONSTANT_False=10
            OP_pushtrue, // CONSTANT_True=11
            OP_pushnull, // CONSTANT_Null=12
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            OP_pushnamespace, //CONSTANT_PackageNamespace=22
            OP_pushnamespace, //CONSTANT_PackageInternalNs=23
            OP_pushnamespace, //CONSTANT_ProtectedNamespace=24
            OP_pushnamespace, //CONSTANT_ExplicitNamespace=25
            OP_pushnamespace, //CONSTANT_StaticProtectedNs=26
            0,
        };


#ifdef AVMPLUS_VERBOSE
        const char * const constantNames[] = {
            "const-0",
            "utf8",//const int CONSTANT_Utf8         = 0x01;
            "const-2",
            "int",//const int CONSTANT_Int = 0x03;
            "uint",//const int CONSTANT_UInt = 0x04;
            "private",//const int CONSTANT_PrivateNS = 0x05;
            "double",//const int CONSTANT_Double       = 0x06;
            "qname",//const int CONSTANT_Qname        = 0x07;
            "namespace",//const int CONSTANT_Namespace    = 0x08;
            "multiname",//const int CONSTANT_Multiname    = 0x09;
            "false",//const inst CONSTANT_False = 0x0A;
            "true",//const int CONSTANT_True         = 0x0B;
            "null",//const int CONSTANT_Null         = 0x0C;
            "@qname",//const int CONSTANT_QnameAttr    = 0x0D;
            "@multiname",//const int CONSTANT_MultinameAttr= 0x0E;
            "rtqname",//const int CONSTANT_RTQname      = 0x0F; // ns::name, var qname, const name
            "@rtqname",//const int CONSTANT_RTQnameA        = 0x10; // @ns::name, var qname, const name
            "rtqnamelate",//const int CONSTANT_RTQnameL     = 0x11; // ns::[], var qname
            "@rtqnamelate",//const int CONSTANT_RTQnameLA   = 0x12; // @ns::[], var qname
            "", //const int CONSTANT_NameL      = 0x13, // o.[], ns=public implied, rt name
            "", //CONSTANT_NameLA       = 0x14, // o.@[], ns=public implied, rt attr-name
            "namespaceset", //CONSTANT_NamespaceSet = 0x15
            "namespace", //PackageNamespace and Namespace are the same as far as the VM is concerned
            "internal",//const int CONSTANT_PackageInternalNS = 0x17
            "protected",//const int CONSTANT_ProtectedNamespace = 0x18
            "explicit",//const int CONSTANT_ExplicitNamespace = 0x19
            "staticprotected",//const int CONSTANT_StaticProtectedNs = 0x1A,
            "multinamelate", //const int CONSTANT_MultinameL        = 0x1B, // o.[], ns, rt name
            "@multinamelate", //CONSTANT_MultinameLA        = 0x1C, // o.@[], ns, rt attr-name
            "typename", //CONSTANT_TypeName = 0x1D
        };

        const char * const traitNames[] = {
            "slot",//const int TRAIT_Slot               = 0x00;
            "method",//const int TRAIT_Method           = 0x01;
            "getter",//const int TRAIT_Getter           = 0x02;
            "setter",//const int TRAIT_Setter           = 0x03;
            "class",//const int TRAIT_Class             = 0x04;
            "",     //obsolete 'function' traits = 0x05
            "const",//const int TRAIT_Const             = 0x06;
        };
#endif // AVMPLUS_VERBOSE

#if defined AVMPLUS_VERBOSE || defined DEBUGGER
#  define N(x)  , x
#else
#  define N(x)
#endif
#if defined VMCFG_WORDCODE
#  define W(x)  , x
#else
#  define W(x)
#endif

        const AbcOpcodeInfo opcodeInfo[] = {
        // For stack movement ("stk") only constant movement is accounted for; variable movement,
        // as for arguments to CALL, CONSTRUCT, APPLYTYPE, et al, and for run-time parts of
        // names, must be handled separately.

        #define ABC_OP(operandCount, canThrow, stack, internalOnly, nameToken)        { operandCount, canThrow, stack W(WOP_##nameToken) N(#nameToken) },
        #define ABC_UNUSED_OP(operandCount, canThrow, stack, internalOnly, nameToken) { operandCount, canThrow, stack W(0)               N(#nameToken) },

        #include "../../core/opcodes.tbl"

        #undef ABC_OP
        #undef ABC_UNUSED_OP
        };

        // Some static asserts to make sure the opcode enum in ActionBlockConstants.h is in good order.
        MMGC_STATIC_ASSERT(OP_0x00 == 0x00);
        MMGC_STATIC_ASSERT(OP_0xFF == 0xFF);
        MMGC_STATIC_ASSERT(OP_end_of_op_codes == 0x100);
    }
}
