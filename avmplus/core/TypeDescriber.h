/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_TypeDescriber__
#define __avmplus_TypeDescriber__

namespace avmplus
{
    class TypeDescriber
    {
    public:
        TypeDescriber(Toplevel* toplevel);

        enum Flags
        {
            // this bit replicates a bug in Flash9/10, where a method that uses a custom namespace
            // won't be in the output if any of its base classes (or interfaces) also define a method
            // in that custom namespace.
            HIDE_NSURI_METHODS      = 0x0001,
            INCLUDE_BASES           = 0x0002,
            INCLUDE_INTERFACES      = 0x0004,
            INCLUDE_VARIABLES       = 0x0008,
            INCLUDE_ACCESSORS       = 0x0010,
            INCLUDE_METHODS         = 0x0020,
            INCLUDE_METADATA        = 0x0040,
            INCLUDE_CONSTRUCTOR     = 0x0080,
            INCLUDE_TRAITS          = 0x0100,
            USE_ITRAITS             = 0x0200,
            HIDE_OBJECT             = 0x0400
        };
        ScriptObject* describeType(Atom value, uint32_t flags);
        Stringp getQualifiedClassName(Atom value);
        Stringp getQualifiedSuperclassName(Atom value);

    private:
        enum StringId
        {
            kstrid_access,
            kstrid_accessors,
            kstrid_api,
            kstrid_asterisk,
            kstrid_bases,
            kstrid_constructor,
            kstrid_declaredBy,
            kstrid_emptyString,
            kstrid_interfaces,
            kstrid_isDynamic,
            kstrid_isFinal,
            kstrid_isStatic,
            kstrid_key,
            kstrid_metadata,
            kstrid_method,
            kstrid_methods,
            kstrid_name,
            kstrid_native,
            kstrid_optional,
            kstrid_parameters,
            kstrid_readonly,
            kstrid_readwrite,
            kstrid_returnType,
            kstrid_traits,
            kstrid_type,
            kstrid_uri,
            kstrid_value,
            kstrid_variables,
            kstrid_writeonly,
            maxStringId
        };

        struct KVPair
        {
            StringId keyid;
            Atom value;
        };

    private:

        Traits* chooseTraits(Atom value, uint32_t flags);
        ScriptObject* new_object();
        ArrayObject* new_array();
        Traitsp getTraits(Atom value);
        Stringp describeClassName(Traitsp traits);
        ScriptObject* describeTraits(Traitsp traits, uint32_t flags, Toplevel* toplevel);
        ArrayObject* describeParams(MethodInfo* mi, MethodSignaturep ms);
        void addDescribeMetadata(ArrayObject* a, PoolObject* pool, const uint8_t* meta_pos);
        ScriptObject* describeMetadataInfo(PoolObject* pool, uint32_t metadata_index);
        Stringp poolstr(PoolObject* pool, uint32_t index);
        void setpropmulti(ScriptObject* o, const KVPair* kv, uint32_t count);
        Stringp str(StringId i);
        static void addBindings(AvmCore* core, MultinameBindingHashtable* bindings, TraitsBindingsp tb, uint32_t flags);

    private:
        Toplevel* m_toplevel;
        Stringp m_strs[maxStringId];
    };
}

#endif /* __avmplus_TypeDescriber__ */
