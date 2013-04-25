/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ActionBlockConstants__
#define __avmplus_ActionBlockConstants__


namespace avmplus
{
    /**
     * Constants for all of the opcodes in the AVM+ instruction set.
     */
    namespace ActionBlockConstants
    {
        /** @name cpool tags */
        /*@{*/
        enum CPoolKind {
            CONSTANT_unused_0x00        = 0x00,
            CONSTANT_Utf8               = 0x01,
#ifdef VMCFG_FLOAT
            CONSTANT_Float              = 0x02,
#endif 
            CONSTANT_Int                = 0x03,
            CONSTANT_UInt               = 0x04,
            CONSTANT_PrivateNs          = 0x05, // non-shared namespace
            CONSTANT_Double             = 0x06,
            CONSTANT_Qname              = 0x07, // o.ns::name, ct ns, ct name
            CONSTANT_Namespace          = 0x08,
            CONSTANT_Multiname          = 0x09, // o.name, ct nsset, ct name
            CONSTANT_False              = 0x0A,
            CONSTANT_True               = 0x0B,
            CONSTANT_Null               = 0x0C,
            CONSTANT_QnameA             = 0x0D, // o.@ns::name, ct ns, ct attr-name
            CONSTANT_MultinameA         = 0x0E, // o.@name, ct attr-name
            CONSTANT_RTQname            = 0x0F, // o.ns::name, rt ns, ct name
            CONSTANT_RTQnameA           = 0x10, // o.@ns::name, rt ns, ct attr-name
            CONSTANT_RTQnameL           = 0x11, // o.ns::[name], rt ns, rt name
            CONSTANT_RTQnameLA          = 0x12, // o.@ns::[name], rt ns, rt attr-name
            CONSTANT_NamespaceSet       = 0x15,
            CONSTANT_PackageNamespace   = 0x16,
            CONSTANT_PackageInternalNs  = 0x17,
            CONSTANT_ProtectedNamespace = 0x18,
            CONSTANT_ExplicitNamespace  = 0x19,
            CONSTANT_StaticProtectedNs  = 0x1A,
            CONSTANT_MultinameL         = 0x1B,
            CONSTANT_MultinameLA        = 0x1C,
            CONSTANT_TypeName           = 0x1D,
#ifdef VMCFG_FLOAT
            CONSTANT_Float4             = 0x1E,
#endif
        };
        /*@}*/

        /** @name traits kinds */
        /*@{*/

        enum TraitKind {
            TRAIT_Slot          = 0x00,
            TRAIT_Method        = 0x01,
            TRAIT_Getter        = 0x02,
            TRAIT_Setter        = 0x03,
            TRAIT_Class         = 0x04,
            TRAIT_Const         = 0x06,
            TRAIT_COUNT         = TRAIT_Const+1,
            TRAIT_mask          = 15
        };
        /*@}*/

        /** @name flags from .abc - limited to a BYTE */
        /*@{*/
        enum AbcMethodFlags
        {
            /** need arguments[0..argc] */
            abcMethod_NEED_ARGUMENTS         = 0x01,

            /** need activation object */
            abcMethod_NEED_ACTIVATION        = 0x02,

            /** need arguments[param_count+1..argc] */
            abcMethod_NEED_REST              = 0x04,

            /** has optional parameters */
            abcMethod_HAS_OPTIONAL           = 0x08,

            /** allow extra args, but dont capture them */
            abcMethod_IGNORE_REST            = 0x10,

            /** method is native */
            abcMethod_NATIVE                 = 0x20,

            /** method sets default namespace */
            abcMethod_SETS_DXNS              = 0x40,

            /** method has table for parameter names */
            abcMethod_HAS_PARAM_NAMES        = 0x80
        };
        /*@}*/

        /** @name attributes */
        /*@{*/
        const int ATTR_final            = 0x10; // 1=final, 0=virtual
        const int ATTR_override         = 0x20; // 1=override, 0=new
        const int ATTR_metadata         = 0x40; // 1=has metadata, 0=no metadata
        /*@}*/

        /** @name opcodes */
        enum AbcOpcode
        {
            #define ABC_OP(operandCount, canThrow, stack, internalOnly, nameToken)        OP_##nameToken,
            #define ABC_UNUSED_OP(operandCount, canThrow, stack, internalOnly, nameToken) ABC_OP(operandCount, canThrow, stack, internalOnly, nameToken)
            #include "opcodes.tbl"
            #undef ABC_OP
            #undef ABC_UNUSED_OP

            //-----
            OP_end_of_op_codes
        };

        struct AbcOpcodeInfo
        {
            int8_t operandCount;    // uses -1 for "invalid", we can avoid that if necessary
            int8_t canThrow;        // always 0 or 1
            int8_t stack;           // stack movement not taking into account run-time names or function arguments
#if defined VMCFG_WORDCODE
            uint16_t wordCode;      // a map used during translation
#endif
#if defined AVMPLUS_VERBOSE || defined DEBUGGER
            const char* name;       // instruction name or OP_0xNN for undefined instructions
#endif
        };

        extern const AbcOpcodeInfo opcodeInfo[];
        extern const AbcOpcodeInfo opcodeInfoNoFloats[];

        extern const unsigned char kindToPushOp[];

#ifdef AVMPLUS_VERBOSE
        /** @name debugger string names */
        /*@{*/
        extern const char * const constantNames[];
        extern const char * const traitNames[];
        /*@}*/
#endif

    }
}

#endif /* __avmplus_ActionBlockConstants__ */
