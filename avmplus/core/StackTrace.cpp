/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
#ifdef DEBUGGER
    void CallStackNode::init(
                    MethodEnv*              env
                    , FramePtr              framep
                    , const uint8_t*        frame_sst
                    , intptr_t volatile*    eip
            )
    {
        AvmAssert(env != NULL);

        if (env->core())
            env->core()->sampleCheck(); // Call sampler before modifying callstack

        m_functionId    = 0;
        m_core          = env->core();
        m_env           = env;
        m_info          = env ? env->method : NULL;
        m_next          = m_core->callStack; m_core->callStack = this;
        m_fakename      = NULL;
        m_depth         = m_next ? (m_next->m_depth + 1) : 1;
        m_eip           = eip;     // ptr to where the current instruction pointer is stored
        m_filename      = NULL;
        m_framep        = framep;
        m_frame_sst     = frame_sst;
        m_linenum       = 0;
    }

    void CallStackNode::init(MethodInfo* methodInfo)
    {
        AvmAssert(methodInfo != NULL);

        m_env           = NULL;
        m_info          = methodInfo;
        m_fakename      = NULL;
        m_core          = NULL;
        m_next          = NULL;
        m_depth         = 0;
        m_eip           = NULL;
        m_filename      = NULL;
        m_framep        = NULL;
        m_frame_sst     = NULL;
        m_linenum       = 0;
    }

    void CallStackNode::init(AvmCore* core, Stringp name)
    {
        // careful, core and/or name can be null
        m_functionId    = 0;
        m_env           = NULL;
        m_info          = NULL;
        m_fakename      = name;
        if (name)
        {
            AvmAssert(core != NULL);

            core->sampleCheck(); // Call sampler before modifying callstack

            m_core      = core;
            m_next      = core->callStack; core->callStack = this;
            m_depth     = m_next ? (m_next->m_depth + 1) : 1;
        }
        else
        {
            m_core      = NULL;
            m_next      = NULL;
            m_depth     = 0;
        }
        m_eip           = 0;
        m_filename      = NULL;
        m_framep        = NULL;
        m_frame_sst     = NULL;
        m_linenum       = 0;
    }

    void CallStackNode::init(AvmCore* core, uint64_t functionId, int32_t lineno)
    {
        AvmAssert(core != NULL);
        AvmAssert(functionId != 0);

        core->sampleCheck(); // Call sampler before modifying callstack

        m_functionId    = functionId;
        m_info          = NULL;
        m_env           = NULL;
        m_fakename      = NULL;
        m_core          = core;
        m_next          = core->callStack; core->callStack = this;
        m_depth         = m_next ? (m_next->m_depth + 1) : 1;
        m_eip           = NULL;
        m_filename      = NULL;
        m_framep        = NULL;
        m_frame_sst     = NULL;
        m_linenum       = lineno;
    }

    CallStackNode::~CallStackNode()
    {
        // The destructor /must not/ do anything except call reset()
        reset();
    }

    bool FASTCALL  CallStackNode::reset()
    {
        AvmCore* core = m_core; // save it since exit() resets to null
        if (core)
        {
            core->sampleCheck(); // Call sampler before popping the callstack
            core->callStack = m_next;
            m_next = NULL;
            m_core = NULL; // so the dtor doesn't pop again
            return true;
        }
        else 
        {
            return false;
        }
    }

    void CallStackNode::enumerateScopeChainAtoms(IScopeChainEnumerator& scb)
    {
        // First, get the "dynamic" portion of the scope chain, that is, the
        // part that is modified on-the-fly within the function.  This includes
        // activation objects for try/catch blocks and "with" clauses.

        if (m_info)
        {
#ifdef VMCFG_AOT
            const int32_t local_count = m_info->local_count();
            const int32_t max_scope = m_info->max_scopes();
            if (m_framep == NULL)
                return;
#else
            MethodSignaturep const ms = m_info->getMethodSignature();
            const int32_t local_count = ms->local_count();
            const int32_t max_scope = ms->max_scope();
#endif
            for (int i = (max_scope + local_count - 1), n = local_count; i >= n; --i)
            {
                Atom const scope = m_info->boxOneLocal(m_framep, i, m_frame_sst);
                AvmAssert(atomKind(scope) != kUnusedAtomTag);
                // go ahead and call addScope, even if null or undefined.
                scb.addScope(scope);
            }
        }

        // Next, get the "static" portion of the scope chain, that is, the
        // part that is defined as part of the definition of the function.  This
        // includes the locals of any functions that enclose this one, and the "this"
        // object, if any.

        ScopeChain* scopeChain = m_env ? m_env->scope() : NULL;
        if (scopeChain)
        {
            int scopeChainLength = scopeChain->getSize();
            for (int i = scopeChainLength - 1; i >= 0; --i)
            {
                Atom scope = scopeChain->getScope(i);
                if (AvmCore::isObject(scope))
                {
                    scb.addScope(scope);
                }
            }
        }
    }
    
#endif
    
    // Dump a filename.  The incoming filename is of the form
    // "C:\path\to\package\root;package/package;filename".  The path format
    // will depend on the platform on which the movie was originally
    // compiled, NOT the platform the the player is running in.
    //
    // We want to replace the semicolons with path separators.  We'll take
    // a guess at the appropriate path separator of the compilation
    // platform by looking for any backslashes in the path.  If there are
    // any, then we'll assume backslash is the path separator.  If not,
    // we'll use forward slash.
    void StackTrace::dumpFilename(Stringp _filename, PrintWriter& out) const
    {
        StringIndexer filename(_filename);

        wchar semicolonReplacement = '/';
        int length = filename->length();
        wchar ch;
        int i;

        // look for backslashes; if there are any, then semicolons will be
        // replaced with backslashes, not forward slashes
        for (i=0; i<length; ++i) {
            ch = filename[i];
            if (ch == '\\') {
                semicolonReplacement = '\\';
                break;
            }
        }

        // output the entire path
        bool previousWasSlash = false;
        for (i=0; i<length; ++i) {
            ch = filename[i];
            if (ch == ';') {
                if (previousWasSlash)
                    continue;
                ch = semicolonReplacement;
                previousWasSlash = true;
            } else if (ch == '/' || ch == '\\') {
                previousWasSlash = true;
            } else {
                previousWasSlash = false;
            }
            out << ch;
        }
    }

    static Stringp getStackTraceLine(MethodInfo* method, Stringp filename)
    {
        AvmCore *core = method->pool()->core;
        StringBuffer sb(core);
        sb << "\tat " << method;
        if (filename)
            sb << "[" << filename << ":";
        return sb.toString();
    }

    Stringp StackTrace::format(AvmCore* core)
    {
        if(!stringRep)
        {
            Stringp s = core->kEmptyString;
            int displayDepth = depth;
            if (displayDepth > kMaxDisplayDepth) {
                displayDepth = kMaxDisplayDepth;
            }
            const Element *e = elements;
            for (int i=0; i<displayDepth; i++, e++)
            {
                // env will be NULL if the element is from a fake CallStackNode
                // omit them since they are only for profiling purposes
                if (!e->info())
                    continue;

                if(i != 0)
                    s = s->appendLatin1("\n");

                Stringp filename=NULL;
                if(e->filename())
                {
                    StringBuffer sb(core);
                    dumpFilename(e->filename(), sb);
                    filename = sb.toString();
                }
                s = core->concatStrings(s, getStackTraceLine(e->info(), filename));
                if(e->filename())
                {
                    s = core->concatStrings(s, core->intToString(e->linenum()));
                    s = s->appendLatin1("]");
                }
#ifdef DEBUGGER
                if(e->info()->file() && e->info()->file()->getName())
                {
                    s = s->appendLatin1(" <");
                    s = core->concatStrings(s, e->info()->file()->getName());
                    s = s->appendLatin1(">");
                }
#endif
            }
            stringRep = s;
        }
        return stringRep;
    }

}
