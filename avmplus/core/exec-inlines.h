/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE MethodEnvProcHolder::MethodEnvProcHolder()
{}

REALLY_INLINE MethodInfoProcHolder::MethodInfoProcHolder()
{}

REALLY_INLINE bool BaseExecMgr::isInterpreted(MethodEnv* env)
{
    return env->method->isInterpreted() != 0;
}

REALLY_INLINE uintptr_t ImtHolder::getIID(MethodInfo* m)
{
    AvmAssert(m->declaringTraits()->isInterface());
#ifdef VMCFG_VERIFYALL
    // In verifyonly mode, make all IIDs be zero to erase memory layout noise.
    return m->pool()->core->config.verifyonly ? 0 : (uintptr_t)m;
#else
    return (uintptr_t) m;
#endif
}

REALLY_INLINE uint32_t ImtHolder::hashIID(uintptr_t iid)
{
    return iid % IMT_SIZE;
}

REALLY_INLINE uint32_t ImtHolder::hashIID(MethodInfo* m)
{
    return hashIID(getIID(m));
}

REALLY_INLINE bool OSR::countEdge(const MethodEnv* env, MethodInfo* m, MethodSignaturep ms)
{
    // Decrement and return true if zero and OSR is supported.  If OSR
    // is not supported, countEdge will remain 0 without triggering OSR,
    // until the next count which will wrap around to 0xFFFFFFFF.  So we only
    // test isSupported() once every 2^32 invocations.
    return --m->_abc.countdown == 0 && isSupported(env->abcEnv(), m, ms);
}

} // namespace avmplus
