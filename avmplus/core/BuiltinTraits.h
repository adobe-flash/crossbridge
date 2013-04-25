/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_BuiltinTraits__
#define __avmplus_BuiltinTraits__

namespace avmplus
{
    // Note: we rely on this being <= 32 entries. None of the enumeration values are magic,
    // so we just keep them in alphabetical order for simplicity.
    enum BuiltinType
    {
        BUILTIN_any,    // this is the "*" type in AS3, corresponds to a NULL Traits in C++ (not the "null" value in AS3")
        BUILTIN_array,
        BUILTIN_boolean,
        BUILTIN_class,  // class Class only, not subclasses of Class
        BUILTIN_date,
        BUILTIN_error,
#ifdef VMCFG_FLOAT
        BUILTIN_float,
        BUILTIN_float4,
#endif 
        BUILTIN_function,
        BUILTIN_int,
        BUILTIN_math,
        BUILTIN_methodClosure,
        BUILTIN_namespace,
        BUILTIN_null,   // this is the "null" AS3 value, not a NULL Traits* in C++
        BUILTIN_number,
        BUILTIN_object, // this is Object specifically, not a subclass thereof
        BUILTIN_qName,
        BUILTIN_regexp,
        BUILTIN_string,
        BUILTIN_uint,
        BUILTIN_vector,
        BUILTIN_vectordouble,
#ifdef VMCFG_FLOAT
        BUILTIN_vectorfloat,
        BUILTIN_vectorfloat4,
#endif
        BUILTIN_vectorint,
        BUILTIN_vectorobj,
        BUILTIN_vectoruint,
        BUILTIN_void,
        BUILTIN_xmlList,
        BUILTIN_xml,

        BUILTIN_none,       // "none of the above" (ie it's not any of the rest of this enum)

        BUILTIN_COUNT
    };

    /**
     * BuiltinTraits is a container for the traits of all of
     * the built-in objects.
     *
     * There may be multiple Toplevel objects, each with their
     * own closures for the core objects.  Multiple Toplevels are
     * used in the Flash Player for security reasons.  Thus, there
     * may be multiple closures for the same core class.
     * There will only be one set of traits, however, and traits
     * are immutable.  So, BuiltinTraits is global to the entire
     * VM.
     */
    class BuiltinTraits
    {
    public:
        BuiltinTraits();

        void initInstanceTypes(PoolObject* pool);
        void initClassTypes(PoolObject* pool);

        // Main built-in class traits
        Traits *array_itraits;
        Traits *boolean_itraits;
        Traits *class_itraits;
        Traits *date_itraits;
        Traits *error_itraits;
#ifdef VMCFG_FLOAT
        Traits *float_itraits;
        Traits *float4_itraits;
#endif 
        Traits *function_itraits;
        Traits *int_itraits;
        Traits *math_itraits;
        Traits *methodClosure_itraits;
        Traits *namespace_itraits;
        Traits *null_itraits;
        Traits *number_itraits;
        Traits *object_itraits;
        Traits *qName_itraits;
        Traits *regexp_itraits;
        Traits *string_itraits;
        Traits *uint_itraits;
        Traits *vector_itraits;
        Traits *vectordouble_itraits;
#ifdef VMCFG_FLOAT
        Traits *vectorfloat_itraits;
        Traits *vectorfloat4_itraits;
#endif
        Traits *vectorint_itraits;
        Traits *vectorobj_itraits;
        Traits *vectoruint_itraits;
        Traits *void_itraits;
        Traits *xmlList_itraits;
        Traits *xml_itraits;

        Traits* object_ctraits;
        Traits* class_ctraits;
        Traits* math_ctraits;
        Traits* int_ctraits;
        Traits* uint_ctraits;
#ifdef VMCFG_FLOAT
        Traits* float_ctraits;
        Traits* float4_ctraits;
#endif 
        Traits* number_ctraits;
        Traits* string_ctraits;
        Traits* boolean_ctraits;
        Traits* vector_ctraits;

    private:
        Traits* findCTraits(const char* name, PoolObject* pool);
    };
}

#endif /* __avmplus_BuiltinTraits__ */
