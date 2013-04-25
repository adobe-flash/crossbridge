/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE PoolObject* PoolObject::create(AvmCore* core, ScriptBuffer& sb, const uint8_t* startpos, ApiVersion apiVersion)
{
    return new (core->GetGC(), MMgc::kExact) PoolObject(core, sb, startpos, apiVersion);
}

REALLY_INLINE const Multiname* PoolObject::precomputedMultiname(int32_t index)
{
    return precompNames->get(index);
}

REALLY_INLINE ApiVersion PoolObject::getApiVersion()
{
    return _apiVersion;
}

REALLY_INLINE const uint8_t* PoolObject::getMetadataInfoPos(uint32_t index)
{
    return metadata_infos[index];
}

REALLY_INLINE Traits* PoolObject::resolveTypeName(const uint8_t*& pc, const Toplevel* toplevel, bool allowVoid)
{
    return resolveTypeName(AvmCore::readU32(pc), toplevel, allowVoid);
}

REALLY_INLINE void PoolObject::resolveBindingNameNoCheck(const uint8_t* &p, Multiname &m, const Toplevel* toplevel) const
{
    resolveBindingNameNoCheck(AvmCore::readU32(p), m, toplevel);
}

REALLY_INLINE void PoolObject::parseMultiname(Multiname& m, uint32_t index) const
{
    parseMultiname(_abcStart + cpool_mn_offsets[index], m);
}

REALLY_INLINE bool PoolObject::hasString(int32_t index) const
{
    // AbcParser stores pointers into all locations
    return (index >= 1 && index < (int32_t) constantStringCount);
}

REALLY_INLINE ScriptBuffer PoolObject::code()
{
    return (ScriptBufferImpl*)_code;
}

REALLY_INLINE bool PoolObject::isCodePointer(const uint8_t* pos)
{
    return pos > &code()[0] && pos < _code->getBuffer() + code().getSize();
}

REALLY_INLINE uint32_t PoolObject::classCount() const
{
    return _classes.length();
}

REALLY_INLINE Traits* PoolObject::getClassTraits(uint32_t i) const
{
    return _classes[i];
}

REALLY_INLINE uint32_t PoolObject::scriptCount() const
{
    return _scripts.length();
}

REALLY_INLINE Traits* PoolObject::getScriptTraits(uint32_t i) const
{
    return _scripts[i];
}

REALLY_INLINE uint32_t PoolObject::methodCount() const
{
    return _methods.length();
}

REALLY_INLINE MethodInfo* PoolObject::getMethodInfo(uint32_t i) const
{
    return _methods[i];
}

REALLY_INLINE int32_t PoolObject::uniqueId() const
{
    AvmAssertMsg(_uniqueId != -1, "id should have been set during parsing");
    return _uniqueId;
}

REALLY_INLINE void PoolObject::setUniqueId(int32_t id)
{
    AvmAssertMsg(_uniqueId == -1, "This method should never be called more than once.");
    _uniqueId = id;
}

#ifdef DEBUGGER
REALLY_INLINE DebuggerMethodInfo* PoolObject::getDebuggerMethodInfo(uint32_t i) const
{
    return (i < _method_dmi.length()) ? _method_dmi[i] : NULL;
}
#endif

#ifdef AVMPLUS_VERBOSE
REALLY_INLINE bool PoolObject::isVerbose(uint32_t flag, MethodInfo* info)
{
    return core->isVerbose(flag, info);
}
#endif

} // namespace avmplus
