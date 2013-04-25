/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Interpreter__
#define __avmplus_Interpreter__

namespace avmplus
{
    // main interpreter method.  Signature should correspond to AtomMethodProc to allow tail
    // calls to here
    Atom interpBoxed(MethodEnv* method, int argc, Atom* ap);

    void** interpGetOpcodeLabels();

    /**
     * callback interface used by interpreter to set up deoptimized frame.
     */
    class FramePopulator {
    public:
        virtual void populate(Atom* framep, int *scope_depth, int *stack_depth) = 0;
        virtual ~FramePopulator() {}
    };

    /**
     * Interpret from the given location, initializing the Atom stack frame
     * from the given values.  This is called by Mason's speculation bailout
     * code in halfmoon.
     */
    Atom interpBoxedAtLocation(int abc_pc, Atom* locals, Atom* operand_stack,
                               Atom* scope_stack, int sp, int scope_height,
                               MethodEnv* env);

    /**
     * Interpreter entry point to begin execution of deoptimized frame.
     * Called by deoptimization trampolines, but not related to speculation
     * bailouts.
     */
    Atom interpBoxedAtLocation(MethodEnv* env, int abc_pc,
                               FramePopulator& populator);

}

//#  define LAST_SUPERWORD_OPCODE    ((50<<8) | OP_ext)

#endif // __avmplus_Interpreter__
