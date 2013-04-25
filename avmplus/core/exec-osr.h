/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_OSR__
#define __avmplus_OSR__

namespace avmplus
{
    /**
     * "On-Stack_Replacement" of interpreter methods with equivalent
     * JIT compiled versions.
     *
     * This is triggered at backwards branches when having looped "often enough"
     * to make amortization of the compilation effort likely.
     *
     * We don't actually *replace* the interpreter frame in question,
     * but simply stack an extra JIT frame on top of it
     * and return from both frames when the method returns.
     * But we call it OSR nevertheless, since that captures the general intent.
     */
    class OSR {
    private:
        static void unboxSlot(FrameState*, MethodEnv*,
                Atom* interpFramePtr, FramePtr jitFramePtr,
                uint8_t* jitFrameTags, int index);

        static bool countInvoke(MethodEnv*);

        // DATA SECTION

        const uint8_t* const osr_pc;        // ABC location of loop label.
        Atom *interp_frame;                 // Interpreter stack frame.
        class FrameState *jit_frame_state;  // FrameState at JIT loop label.

        // END DATA SECTION

    public:
        /** Create a new on-stack-replacement record. */
        OSR(const uint8_t* osr_pc, Atom* interp_frame);

        /** Save loop-entry frame state; called by JIT during compilation. */
        void setFrameState(FrameState* fs);

        const uint8_t* osrPc() const;

        /**
         * Parse an OSR configuration string, return the osr_threshold value,
         * or -1 if the string is invalid.
         */
        static int32_t parseConfig(const char *str);

        /** Return true if we can do OSR for this function, false otherwise. */
        static bool isSupported(const AbcEnv*, const MethodInfo*, MethodSignaturep);

        /** Called by the interpreter to trigger OSR. */
        static bool countEdge(const MethodEnv* env, MethodInfo* m, MethodSignaturep ms);

        /**
         * Called by the interpreter to perform OSR and continue execution in
         * JIT-compiled code.
         */
        static bool execute(MethodEnv *env, Atom *interp_frame,
                            MethodSignaturep ms, const uint8_t* osr_pc, Atom* result);

        /**
         * Called by OSR-instrumented JIT-generated code to test whether to
         * jump into a loop, and populate the JIT stack frame with values
         * from the interpreter stack frame.
         */
        static void adjustFrame(MethodFrame*, CallStackNode*,
                                FramePtr jitFramePointer, uint8_t *jitFrameTags);

        #ifdef DEBUG
        // Verify that currentBugCompatibility() is as OSR expected when method is called.
        static void checkBugCompatibility(MethodEnv* env);
        #endif

        // Trampolines, referenced by setInterp():
        static uintptr_t osrInterpGPR(MethodEnv* method, int argc, uint32_t *ap);
        static double osrInterpFPR(MethodEnv* method, int argc, uint32_t *ap);
#ifdef VMCFG_FLOAT
        static float4_t osrInterpVECR(MethodEnv* method, int argc, uint32_t *ap);
#endif
        static Atom osrInvokeInterp(MethodEnv* env, int32_t argc, Atom* argv);

        // Three more trampolines to handle the init variants:
        static uintptr_t osrInitInterpGPR(MethodEnv* method, int argc, uint32_t *ap);
        static double osrInitInterpFPR(MethodEnv* method, int argc, uint32_t *ap);
#ifdef VMCFG_FLOAT
        static float4_t osrInitInterpVECR(MethodEnv* method, int argc, uint32_t *ap);
#endif
        static Atom osrInitInvokeInterp(MethodEnv* env, int32_t argc, Atom* argv);
    };
}

#endif // __avmplus_OSR__
