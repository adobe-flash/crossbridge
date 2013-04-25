/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE bool FrameState::typeNotNull(Traits* t) {
    switch (Traits::getBuiltinType(t)) {
        case BUILTIN_int:
        case BUILTIN_uint:
        case BUILTIN_number:
        case BUILTIN_boolean:
            return true;
        default:
            return false;
    }
}

REALLY_INLINE FrameValue& FrameState::value(int32_t i)
{
    AvmAssert(i >= 0 && i < frameSize);
    return locals[i];
}

REALLY_INLINE const FrameValue& FrameState::value(int32_t i) const
{
    AvmAssert(i >= 0 && i < frameSize);
    return locals[i];
}

REALLY_INLINE FrameValue& FrameState::scopeValue(int32_t i)
{
    AvmAssert(i >= 0 && i < scopeDepth);
    return value(scopeBase + i);
}

REALLY_INLINE const FrameValue& FrameState::scopeValue(int32_t i) const
{
    AvmAssert(i >= 0 && i < scopeDepth);
    return value(scopeBase + i);
}

REALLY_INLINE FrameValue& FrameState::stackValue(int32_t i)
{
    AvmAssert(i >= 0 && i < stackDepth);
    return value(stackBase + i);
}

REALLY_INLINE FrameValue& FrameState::stackTop()
{
    return value(stackBase + stackDepth - 1);
}

REALLY_INLINE int32_t FrameState::sp() const
{
    return stackBase + stackDepth - 1;
}

REALLY_INLINE void FrameState::setType(int32_t i, Traits* t)
{
    setType(i, t, typeNotNull(t));
}

REALLY_INLINE void FrameState::setType(int32_t i, Traits* t, bool notNull, bool isWith)
{
    FrameValue& v = value(i);
    v.traits = t;
    v.notNull = notNull;
    v.isWith = isWith;
#ifdef VMCFG_NANOJIT
    BuiltinType bt = Traits::getBuiltinType(t);
    v.sst_mask = 1 << valueStorageType(bt);

FLOAT_ONLY( 
    if(bt == BUILTIN_float4) info->forceLargeVarSize();
)
#endif
}

REALLY_INLINE void FrameState::pop(int32_t n)
{
    stackDepth -= n;
    AvmAssert(stackDepth >= 0);
}

REALLY_INLINE FrameValue& FrameState::peek(int32_t n)
{
    return value(stackBase + stackDepth - n);
}

REALLY_INLINE const FrameValue& FrameState::peek(int32_t n) const
{
    return value(stackBase + stackDepth - n);
}

REALLY_INLINE void FrameState::pop_push(int32_t n, Traits* t)
{
    pop_push(n, t, typeNotNull(t));
}

REALLY_INLINE void FrameState::pop_push(int32_t n, Traits* type, bool notNull)
{
    int32_t sp = stackDepth - n;
    setType(stackBase + sp, type, notNull);
    stackDepth = sp+1;
}

REALLY_INLINE void FrameState::push(FrameValue& value)
{
    AvmAssert(stackBase + stackDepth + 1 <= frameSize);
    setType(stackBase + stackDepth++, value.traits, value.notNull);
}

REALLY_INLINE void FrameState::push(Traits* traits)
{
    push(traits, typeNotNull(traits));
}

REALLY_INLINE void FrameState::push(Traits* traits, bool notNull)
{
    AvmAssert(stackBase + stackDepth+1 <= frameSize);
    setType(stackBase + stackDepth++, traits, notNull);
}
} // namespace avmplus
