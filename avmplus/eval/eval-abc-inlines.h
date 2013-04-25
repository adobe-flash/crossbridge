/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

inline uint32_t ABCFile::addMethod(ABCMethodInfo* m)
{
    methods.addAtEnd(m);
    return methodCount++;
}

inline uint32_t ABCFile::addMetadata(ABCMetadataInfo* m)
{
    metadatas.addAtEnd(m);
    return metadataCount++;
}

inline uint32_t ABCFile::addClassAndInstance(ABCClassInfo* c, ABCInstanceInfo* i)
{
    uint32_t x = addClass(c);
    DEBUG_ONLY( uint32_t y = ) addInstance(i);
    AvmAssert( x == y );
    return x;
}

inline uint32_t ABCFile::addInstance(ABCInstanceInfo* i)
{
    instances.addAtEnd(i);
    return instanceCount++;
}

inline uint32_t ABCFile::addClass(ABCClassInfo* c)
{
    classes.addAtEnd(c);
    return classCount++;
}

inline uint32_t ABCFile::addScript(ABCScriptInfo* s)
{
    scripts.addAtEnd(s);
    return scriptCount++;
}

// The offset must not be revealed - the body may not actually be serialized
inline void ABCFile::addMethodBody(ABCMethodBodyInfo* m)
{
    bodies.addAtEnd(m);
    if (!m->is_empty)
        nonemptyMethodBodyCount++;
}

inline uint8_t ABCMethodBodyInfo::getFlags()
{
    return cogen.getFlags();
}

inline void ABCMethodInfo::setFlags(uint8_t flags)
{
    AvmAssert(this->flags == ~0U);
    this->flags = flags;
}

inline uint32_t ABCTraitsTable::addTrait(ABCTrait* t)
{
    traits.addAtEnd(t);
    return traitsCount++;
}

inline uint32_t ABCFile::addQName(uint32_t ns, uint32_t name, bool is_attr)
{
    using namespace ActionBlockConstants;
    return multinameLookup((uint8_t)(is_attr ? CONSTANT_QnameA : CONSTANT_Qname), ns, name);
}

inline uint32_t ABCFile::addRTQName(uint32_t name, bool is_attr)
{
    using namespace ActionBlockConstants;
    return multinameLookup((uint8_t)(is_attr ? CONSTANT_RTQnameA : CONSTANT_RTQname), NO_VALUE, name);
}

inline uint32_t ABCFile::addRTQNameL(bool is_attr)
{
    using namespace ActionBlockConstants;
    return multinameLookup((uint8_t)(is_attr ? CONSTANT_RTQnameLA : CONSTANT_RTQnameL), NO_VALUE, NO_VALUE);
}

inline uint32_t ABCFile::addMultiname(uint32_t nsset, uint32_t name, bool is_attr)
{
    using namespace ActionBlockConstants;
    return multinameLookup((uint8_t)(is_attr ? CONSTANT_MultinameA : CONSTANT_Multiname), nsset, name);
}

inline uint32_t ABCFile::addMultinameL(uint32_t nsset, bool is_attr)
{
    using namespace ActionBlockConstants;
    return multinameLookup((uint8_t)(is_attr ? CONSTANT_MultinameLA : CONSTANT_MultinameL), nsset, NO_VALUE);
}

inline uint32_t ABCFile::addTypeName(uint32_t parameterizedType, uint32_t parameterType)
{
    using namespace ActionBlockConstants;
    return multinameLookup(CONSTANT_TypeName, parameterizedType, parameterType);
}
    
inline void ABCMethodBodyInfo::clearTraits()
{
    traits = NULL;
}

inline uint32_t ABCExceptionTable::addAtEnd(ABCExceptionInfo* e)
{
    exceptions.addAtEnd(e);
    return exceptionCount++;
}
}}
