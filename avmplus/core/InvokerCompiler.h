/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_InvokerCompiler__
#define __avmplus_InvokerCompiler__

namespace avmplus
{
    using namespace nanojit;

    /**
     * compiles MethodEnv::coerceEnter, specialized for the method being
     * called, where the expected arg count and argument types are known.  This
     * allows all the arg coersion to be unrolled and specialized.
     *
     * Native methods and JIT methods are compiled in this way.  Interpreted
     * methods do not gain enough from compilation since the method bodies are
     * interpreted, and since arguments only need to be coerced, not unboxed.
     *
     * Compilation occurs on the second invocation.
     */
    class InvokerCompiler: public LirHelper {
    public:
        // true if we are able to compile an invoker for this method
        static bool canCompileInvoker(MethodInfo* info);

        // main compiler driver
        static AtomMethodProc compile(MethodInfo* info);

    private:
        // sets up compiler pipeline
        InvokerCompiler(MethodInfo*);

        // compiler front end; generates LIR
        void generate_lir();

        // compiler back end; generates native code from LIR
        void* assemble();

        // generates argc check
        void emit_argc_check(LIns* argc_param);

        // downcast and unbox (and optionally copy) each arg
        void downcast_args(LIns* env_param, LIns* argc_param, LIns* args_param);

        // downcast and unbox one arg
        void downcast_arg(int arg, int offset, LIns* env_param, LIns* args_param);

        // downcast and unbox the value for the given type and store result in args_out
        void downcast_and_store(LIns* val, Traits* t, LIns* env, int offset);

        // generate code to call the underlying method directly
        void call_method(LIns* env_param, LIns* argc_param);

        // is verbose-mode enabled?
        bool verbose();

        // should unmodified args be copied?  true if we are not coercing in-place
        bool copyArgs(); // true if un-modified args must be copied anyway

    private:
        MethodInfo* method;     // MethodInfo for method that we will call
        MethodSignaturep ms;    // the signature for same
        LIns* maxargs_br;       // branch that tests for too many args
        LIns* minargs_br;       // branch that tests for not enough args
        LIns* args_out;         // separate allocation for outgoing args (optional, NULL if unused)
    };
}
#endif /* __avmplus_InvokerCompiler__ */
