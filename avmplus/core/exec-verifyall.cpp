/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus {

// Called after ABC is loaded before anything else happens.  We have a
// chance here to kick off eager verification.
void BaseExecMgr::notifyAbcPrepared(Toplevel* toplevel, AbcEnv* abcEnv)
{
#ifdef VMCFG_VERIFYALL
    if (config.verifyall) {
        PoolObject* pool = abcEnv->pool();
        for (int i=0, n=pool->scriptCount(); i < n; i++)
            enqTraits(pool->getScriptTraits(i));
        verifyEarly(toplevel, abcEnv);
    }
#endif
    (void) toplevel;
    (void) abcEnv;
}

#ifdef VMCFG_VERIFYALL
VerifyallWriter::VerifyallWriter(MethodInfo* m, BaseExecMgr* exec, CodeWriter *coder)
    : NullWriter(coder)
    , pool(m->pool())
    , exec(exec)
{}

void VerifyallWriter::write(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, Traits *type)
{
    if (opcode == OP_newactivation)
        exec->enqTraits(type);
    coder->write(state, pc, opcode, type);
}

void VerifyallWriter::writeOp1(const FrameState* state, const uint8_t *pc, AbcOpcode opcode, uint32_t opd1, Traits *type)
{
    if (opcode == OP_newfunction) {
        MethodInfo *f = pool->getMethodInfo(opd1);
        AvmAssert(f->declaringTraits() == type);
        exec->enqFunction(f);
        exec->enqTraits(type);
    }
    else if (opcode == OP_newclass) {
        exec->enqTraits(type);
        exec->enqTraits(type->itraits);
    }
    coder->writeOp1(state, pc, opcode, opd1, type);
}

// True after a method is verified.
REALLY_INLINE bool BaseExecMgr::isVerified(const MethodInfo* m) const
{
    return m->_isVerified != 0;
}

// True when a method is in verifyFunctionQueue, reset to false once verified.
REALLY_INLINE bool BaseExecMgr::isVerifyPending(const MethodInfo* m) const
{
    return m->_isVerifyPending != 0;
}

REALLY_INLINE void BaseExecMgr::setVerified(MethodInfo* m) const
{
    m->_isVerified = 1;
    m->_isVerifyPending = 0;
}

REALLY_INLINE void BaseExecMgr::setVerifyPending(MethodInfo* m) const
{
    m->_isVerifyPending = 1;
}

// Add a function to the verify queue if not already queued or verified.
void BaseExecMgr::enqFunction(MethodInfo* f)
{
    if (f != NULL && !isVerified(f) && !isVerifyPending(f)) {
        setVerifyPending(f);
        verifyFunctionQueue.add(f);
    }
}

// Add a Traits to verifyTraitsQueue if not already queued.
void BaseExecMgr::enqTraits(Traits* t)
{
    if (!t->isInterface() && verifyTraitsQueue.indexOf(t) < 0)
        verifyTraitsQueue.add(t);
}

// In verifyonly mode, this invoker is used for all functions, ensuring
// that no code actually runs.
Atom verifyOnlyInvoker(MethodEnv*, int32_t, Atom*)
{
    return undefinedAtom;
}

// After loading an ABC and inserting scripts into the verify queue,
// process the work queues until they are empty.
void BaseExecMgr::verifyEarly(Toplevel* toplevel, AbcEnv* abc_env)
{
    GCList<MethodInfo> verifyQueue2(core->GetGC(), kListInitialCapacity);
    int verified;
    do {
        verified = 0;
        while (!verifyTraitsQueue.isEmpty()) {
            Traits* t = verifyTraitsQueue.removeFirst();
            t->resolveSignatures(toplevel);
            TraitsBindingsp td = t->getTraitsBindings();
            enqFunction(t->init);
            for (int i=0, n=td->methodCount; i < n; i++)
                enqFunction(td->getMethod(i));
        }
        while (!verifyFunctionQueue.isEmpty()) {
            MethodInfo* f = verifyFunctionQueue.removeLast();
            if (!isVerified(f)) {
                if (f->declaringTraits()->init != f && f->declaringScope() == NULL) {
                    verifyQueue2.add(f);
                    continue;
                }
                verified++;
                //console << "pre verify " << f << "\n";
                verifyMethod(f, toplevel, abc_env);
                setVerified(f);
                if (config.verifyonly)
                    f->_invoker = verifyOnlyInvoker;
            }
        }
        while (!verifyQueue2.isEmpty())
            verifyFunctionQueue.add(verifyQueue2.removeLast());
    } while (verified > 0);
}

#endif // VMCFG_VERIFYALL

} // namespace avmplus

