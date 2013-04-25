/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_CodeContext__
#define __avmplus_CodeContext__

namespace avmplus
{
    // CodeContext is used to track which security context we are in.
    // When an AS3 method is called, the AS3 method will ensure that core->codeContext() will return its context.
    // Note that CodeContext should not be instantiated directly (except in certain situations
    // by AvmCore); client code should create a concrete subclass.
    class CodeContext : public MMgc::GCObject
    {
        friend class AvmCore;

    protected:
        explicit CodeContext(DomainEnv* env, const BugCompatibility* bugCompatibility)
            : m_domainEnv(env)
            , m_bugCompatibility(bugCompatibility)
        {
            AvmAssert(env != NULL);
        }

    public:
        REALLY_INLINE DomainEnv* domainEnv() const { return m_domainEnv; }
        REALLY_INLINE const BugCompatibility* bugCompatibility() const { return m_bugCompatibility; }

    private:
        GCMember<DomainEnv>                 m_domainEnv;
        GCMember<const BugCompatibility>    m_bugCompatibility;
    };

    class EnterCodeContext
    {
    public:
        EnterCodeContext() : m_core(NULL) {}  // support dynamic entry

        explicit EnterCodeContext(AvmCore* core, CodeContext* new_cc) : m_core(NULL)
        {
            enter(core,new_cc);
        }
        void enter(AvmCore* core, CodeContext* new_cc)
        {
            AvmAssert(core != NULL);
            AvmAssert(m_core == NULL); // Don't permit multiple entry.
            m_core = core;
            m_frame.enter(core, new_cc);
            // fix for https://bugzilla.mozilla.org/show_bug.cgi?id=537980
            // ensure that dxns has a suitable default value for artifical MethodFrames
            m_frame.setDxns(core->publicNamespace);
        }
        ~EnterCodeContext()
        {
            if (m_core) {
                m_frame.exit(m_core);
            }
        }
    private:

        AvmCore* m_core;
        MethodFrame m_frame;
    };


}

#endif /* __avmplus_CodeContext__ */
