/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    BuiltinTraits::BuiltinTraits()
    {
        VMPI_memset(this, 0, sizeof(BuiltinTraits));
    }

    // this is called after core types are defined.  we don't want to do
    // it earlier because we need Void and int
    void BuiltinTraits::initInstanceTypes(PoolObject* pool)
    {
        AvmCore* core = pool->core;

        Namespacep publicNS = core->getPublicNamespace(kApiVersion_VM_ALLVERSIONS);

        null_itraits = Traits::newTraits(pool, NULL, 0, 0, 0, TRAITSTYPE_NVA);
        null_itraits->set_names(publicNS, core->knull);
        null_itraits->final = true;
        null_itraits->builtinType = BUILTIN_null;
        null_itraits->verifyBindings(NULL);
        null_itraits->resolveSignatures(NULL);

        void_itraits = Traits::newTraits(pool, NULL, 0, 0, 0, TRAITSTYPE_NVA);
        void_itraits->set_names(publicNS, core->kvoid);
        void_itraits->final = true;
        void_itraits->builtinType = BUILTIN_void;
        void_itraits->verifyBindings(NULL);
        void_itraits->resolveSignatures(NULL);
        
#define DO_BUILTIN(nm, clsnm) \
    do { \
        nm##_itraits = core->domainMgr()->findBuiltinTraitsByName(pool, core->internConstantStringLatin1(clsnm)); \
        nm##_itraits->builtinType = BUILTIN_##nm; \
    } while (0)

        DO_BUILTIN(array, "Array");
        DO_BUILTIN(boolean, "Boolean");
        DO_BUILTIN(class, "Class");
        DO_BUILTIN(date, "Date");
        DO_BUILTIN(error, "Error");
        DO_BUILTIN(function, "Function");
        DO_BUILTIN(int, "int");
        DO_BUILTIN(math, "Math");
        DO_BUILTIN(methodClosure, "MethodClosure");
        DO_BUILTIN(namespace, "Namespace");
//      DO_BUILTIN(null, "FOO");        // handled above
        DO_BUILTIN(number, "Number");
#ifdef VMCFG_FLOAT
        DO_BUILTIN(float, "float");
        DO_BUILTIN(float4, "float4");
#endif 
//      DO_BUILTIN(object, "FOO");      // can't do yet, handled elsewhere
        DO_BUILTIN(qName, "QName");
        DO_BUILTIN(regexp, "RegExp");
        DO_BUILTIN(string, "String");
        DO_BUILTIN(uint, "uint");
        DO_BUILTIN(vector, "Vector");
        DO_BUILTIN(vectordouble, "Vector$double");
#ifdef VMCFG_FLOAT
        DO_BUILTIN(vectorfloat, "Vector$float");
        DO_BUILTIN(vectorfloat4, "Vector$float4");
#endif
        DO_BUILTIN(vectorint, "Vector$int");
        DO_BUILTIN(vectorobj, "Vector$object");
        DO_BUILTIN(vectoruint, "Vector$uint");
//      DO_BUILTIN(void, "FOO");        // handled above
        DO_BUILTIN(xmlList, "XMLList");
        DO_BUILTIN(xml, "XML");
#undef DO_BUILTIN

        // we have fast equality checks in the core that only work
        // because these two classes are final.  If they become non-final
        // then these checks need to be updated to account for possible
        // subclassing.
        AvmAssert(xml_itraits->final);
        AvmAssert(xmlList_itraits->final);

        // XML and XMLList are dynamic but do not need the
        // standard dynamic hash table
        xml_itraits->set_needsHashtable(false);
        xmlList_itraits->set_needsHashtable(false);

        vectordouble_itraits->set_names(vectordouble_itraits->ns(), core->kVectorNumber);
#ifdef VMCFG_FLOAT
        vectorfloat_itraits->set_names(vectorfloat_itraits->ns(), core->kVectorFloat);
        vectorfloat4_itraits->set_names(vectorfloat4_itraits->ns(), core->kVectorFloat4);
#endif
        vectorint_itraits->set_names(vectorint_itraits->ns(), core->kVectorint);
        vectoruint_itraits->set_names(vectoruint_itraits->ns(), core->kVectoruint);
        vectorobj_itraits->set_names(vectorobj_itraits->ns(), core->kVectorAny);
    }

    void BuiltinTraits::initClassTypes(PoolObject* pool)
    {
        object_ctraits = findCTraits("Object$", pool);
        class_ctraits = findCTraits("Class$", pool);
        math_ctraits = findCTraits("Math$", pool);
        number_ctraits = findCTraits("Number$", pool);
#ifdef VMCFG_FLOAT
        float_ctraits = findCTraits("float$", pool);
        float4_ctraits = findCTraits("float4$", pool);
#endif // VMCFG_FLOAT
        int_ctraits = findCTraits("int$", pool);
        uint_ctraits = findCTraits("uint$", pool);
        boolean_ctraits = findCTraits("Boolean$", pool);
        string_ctraits = findCTraits("String$", pool);
        vector_ctraits = findCTraits("Vector$", pool);
    }

    Traits* BuiltinTraits::findCTraits(const char* cname, PoolObject* pool)
    {
        Stringp name = pool->core->internConstantStringLatin1(cname);
        for (uint32_t i=0, n=pool->classCount(); i < n; i++)
        {
            Traits* ctraits = pool->getClassTraits(i);
            if (ctraits && ctraits->name() == name)
            {
                return ctraits;
            }
        }
        return NULL;
    }
}
