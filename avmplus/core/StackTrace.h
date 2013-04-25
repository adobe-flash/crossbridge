/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_CallStackNode__
#define __avmplus_CallStackNode__

namespace avmplus
{
    /**
     * The CallStackNode class tracks the call stack of executing
     * ActionScript code.  It is used for debugger stack dumps
     * and also to keep track of the security contexts of
     * running code.  There is a master CallStackNode object on
     * AvmCore, which is maintained by MethodInfo::enter as
     * methods enter and exit.
     *
     * A single CallStackNode object represents a level of the
     * call stack.  It tracks the currently executing method,
     * and the source file and line number if debug info
     * is available.
     *
     * CallStackNode objects get strung together into linked
     * lists to represent a full call stack.  Entering a
     * method adds a new element to core->stackTrace's
     * linked list; exiting a method removes it.
     *
     * As OP_debugfile and OP_debugline opcodes are encountered,
     * the file and line number information in the current
     * CallStackNode object is updated.
     *
     * CallStackNode objects are "semi-immutable."  The file
     * and line number information can be updated until someone
     * wants to snapshot the CallStackNode.  To snapshot, the
     * "freeze" method is called.  This freezes the entire
     * linked list.  Once a CallStackNode is frozen, it cannot
     * be directly modified... a clone must be made of it,
     * and then the clone can be modified.
     *
     * The "semi-immutable" aspect is to support AVM+ heap
     * profiling.  Every ScriptObject will point to the
     * CallStackNode at time of allocation, so that the developer
     * can figure out the call stack where an object was
     * created.  The semi-immutability makes it possible to
     * conserve memory, by having many ScriptObjects point to
     * the same CallStackNode chain, or chains containing common
     * CallStackNode elements.
     */
    class CallStackNode
    {
    public:

        // Note that the framep you hand to CallStackNode
        // varies depending on whether the Method
        // in question is jitted or interped: if jitted, framep points
        // to a series of 8-byte native values. if interped, framep
        // points to a series of Atom.

        void init(MethodEnv*                env
                    , FramePtr              framep
                    , const uint8_t*        frame_sst
                    , intptr_t volatile*    eip
                );

        void init(MethodInfo* methodInfo);

        void init(AvmCore* core, Stringp name);

        /**
         * Constructs a new CallStackNode.  The method executing and
         * the CallStackNode representing the previous call stack
         * must be specified.  The source file and line number
         * will be unset, initially.
         */
        inline explicit CallStackNode(MethodEnv*                env
                                        , FramePtr              framep
                                        , const uint8_t*        frame_sst
                                        , intptr_t volatile*    eip
                                )
        {
            init(env, framep, frame_sst, eip);
        }

        // ctor used for external script profiling
        inline explicit CallStackNode(AvmCore* core, uint64_t functionId, int32_t lineno)
        {
            init(core, functionId, lineno);
        }

        void init(AvmCore* core, uint64_t functionId, int32_t lineno);

#ifdef DEBUGGER
        // ctor used only for Sampling (no MethodEnv)
        inline explicit CallStackNode(AvmCore* core, const char* name)
        {
            Sampler* sampler = core ? core->get_sampler() : NULL;
            init(core, sampler ? sampler->getFakeFunctionName(name) : NULL);
        }
#endif

        // ctor used only for MethodInfo::verify (no MethodEnv, but has MethodInfo)
        inline explicit CallStackNode(MethodInfo* info)
        {
            init(info);
        }

        // dummy ctor we can use to construct an uninitalized version -- useful for the thunks, which
        // will construct one and immediately call initialize (via debugEnter) so there's no need to
        // redundantly fill in fields. (note that the fields are uninitialized so you *must* call init
        // afterwards when using this!)
        enum Empty { kEmpty = 0 };
        inline explicit CallStackNode(Empty) { }

        // dummy ctor used by verify to skip initialization in non-debugger runmode.
        // you don't need to follow this with init (but can if you like)
        enum NoOp { kNoOp = 0 };
        inline explicit CallStackNode(NoOp) { VMPI_memset(this, 0, sizeof(*this)); }

        ~CallStackNode();

        // Does exactly what the destructor does, but on an arbitrary object.
        bool FASTCALL reset();

#ifdef DEBUGGER
        inline void sampleCheck() { if (m_core) m_core->sampleCheck(); }
#endif

        inline void setNext(CallStackNode* next) { m_next = next; }
        inline CallStackNode* next() const { return m_next; }
        // WARNING, env() can return null if there are fake Sampler-only frames. You must always check for null.
        inline MethodEnv* env() const { return m_env; }
        // WARNING, info() can return null if there are fake Sampler-only frames. You must always check for null.
        inline MethodInfo* info() const { return m_info; }
        inline Stringp fakename() const { return m_fakename; }
        inline int32_t depth() const { return m_depth; }

        inline intptr_t volatile* eip() const { return m_eip; }
        inline Stringp filename() const { return m_filename; }
        inline FramePtr framep() const { return m_framep; }
        inline const uint8_t* types() const { return m_frame_sst; }
        inline int32_t linenum() const { return m_linenum; }

        inline void set_filename(Stringp s) { m_filename = s; }
        inline void set_linenum(int32_t i) { m_linenum = i; }

        inline uint64_t functionId() const { return m_functionId; }
        inline bool isAS3Sample() const { return m_functionId == 0; }

        // Placement new and delete because the interpreter allocates CallStackNode
        // instances inside other data structures (think alloca storage that has been
        // allocated on the heap).
        //
        // The delete operator is required by some compilers in builds that compile with
        // exceptions enabled, but is never actually called.

        inline void* operator new(size_t, void* storage) { return storage; }
        inline void operator delete(void*) {}

        // enumerateScopeChainAtoms will list the active scopechain by repeatedly calling the addScope of the interface.
        class IScopeChainEnumerator
        {
        public:
            virtual ~IScopeChainEnumerator() { }
            virtual void addScope(Atom scope) = 0;
        };
        void enumerateScopeChainAtoms(IScopeChainEnumerator& scb);

    // ------------------------ DATA SECTION BEGIN
    private:    uint64_t            m_functionId;   // int used to uniquely identify function calls in external scripting languages
    private:    AvmCore*            m_core;
    private:    MethodEnv*          m_env;          // will be NULL if the element is from a fake CallStackNode
    private:    MethodInfo*         m_info;
    private:    CallStackNode*      m_next;
    private:    Stringp             m_fakename;     // NULL unless we are a fake CallStackNode
    private:    intptr_t volatile*  m_eip;          // ptr to where the current pc is stored
    private:    Stringp             m_filename;     // in the form "C:\path\to\package\root;package/package;filename"
    private:    FramePtr            m_framep;       // pointer to top of AS registers
    private:    const uint8_t*      m_frame_sst;    // array of traits for AS registers and scopechain
    private:    int32_t             m_linenum;
    private:    int32_t             m_depth;
    // ------------------------ DATA SECTION END
    };

    class GC_CPP_EXACT(StackTrace, MMgc::GCTraceableObject)
    {
        StackTrace(int depth) : depth(depth) {}

    public:
        REALLY_INLINE static StackTrace* create(MMgc::GC* gc, int depth)
        {
            size_t extra = depth > 0 ? sizeof(StackTrace::Element) * (depth-1) : 0;
            return new (gc, MMgc::kExact, extra) StackTrace(depth);
        }

        Stringp format(AvmCore* core);

        struct Element
        {
            enum { EXTERNAL_CALL_FRAME = 1 }; // used for CallStackNodes for external scripting languages

            MethodInfo*         m_info;         // will be null for fake CallStackNode
            int32_t             m_linenum;
        #ifdef VMCFG_64BIT
            int32_t             m_pad;
        #endif
            union
            {
                struct
                {
                    Stringp     m_fakename;     // needed just for fake CallStackNodes, null otherwise
                    Stringp     m_filename;     // in the form "C:\path\to\package\root;package/package;filename"
                } u;
                uint64_t        m_functionId;
            };

            inline void set(const CallStackNode& csn)
            {
                if (csn.functionId() != 0)
                {
                    m_info      = (MethodInfo*) EXTERNAL_CALL_FRAME;
#ifdef VMCFG_64BIT
                    u.m_fakename    = NULL;         // let's keep the stack nice and clean
#endif
                    m_functionId = csn.functionId();
                }
                else
                {
                    m_info      = csn.info();       // will be NULL if the element is from a fake CallStackNode
                    u.m_fakename    = csn.fakename();
                    u.m_filename    = csn.filename();
                }
                m_linenum   = csn.linenum();
#ifdef VMCFG_64BIT
                m_pad       = 0;    // let's keep the stack nice and clean
#endif
            }
            
            inline void set(MethodInfo* methodInfo)
            {

                m_info = methodInfo;       // will be NULL if the element is from a fake CallStackNode
                u.m_fakename = NULL;
                u.m_filename = NULL;
                m_linenum = 0;
#ifdef VMCFG_64BIT
                m_pad       = 0;    // let's keep the stack nice and clean
#endif
            }
            
            inline void gcTrace(MMgc::GC* gc)
            {
                gc->TraceLocation(&m_info);
                gc->TraceLocation(&u.m_filename);
            }

            inline bool isAS3Sample() const { return m_info != (MethodInfo*) EXTERNAL_CALL_FRAME; }
            // WARNING, info() can return null if there are fake Sampler-only frames. You must always check for null.
            inline MethodInfo* info() const { return isAS3Sample() ? m_info : NULL; }
#ifdef _DEBUG
            // used in Sampler::presweep to check if the fake name is null or marked
            inline Stringp fakename() const { return isAS3Sample() ? u.m_fakename : NULL; }
#endif
            inline Stringp name() const     { return isAS3Sample() ? ((!u.m_fakename && m_info) ? m_info->getMethodName() : u.m_fakename) : NULL; }
            inline Stringp filename() const { return isAS3Sample() ? u.m_filename : NULL; }
            inline int32_t linenum() const { return m_linenum; }
            inline uint64_t functionId() const { return isAS3Sample() ? 0 : m_functionId; }
        };

        bool equals(StackTrace::Element* e, int depth);
        static uintptr_t hashCode(StackTrace::Element* e, int depth);

    private:
        void dumpFilename(Stringp filename, PrintWriter& out) const;
        
    public:
        /**
         * The dump and format methods display a human-readable
         * version of the stack trace.  To avoid thousands of
         * lines of output, such as would happen in an infinite
         * recursion, no more than kMaxDisplayDepth levels will
         * be displayed.
         */
        const static int kMaxDisplayDepth = 64;

        // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(StackTrace)
        
        const int        depth;
        GCMember<String> GC_POINTER(stringRep);
        Element          GC_STRUCTURES(elements[1], depth);
        
        GC_DATA_END(StackTrace)
        // ------------------------ DATA SECTION END
    };

}

#endif /* __avmplus_CallStackNode__ */
