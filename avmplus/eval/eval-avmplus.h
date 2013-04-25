/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This is the public API between eval and avmplus.

#ifdef VMCFG_EVAL

namespace avmplus
{
    /**
     * Compile 'code' to ABC and return the ABC in a ScriptBuffer.
     *
     * @param core The core used for allocation etc
     * @param toplevel The toplevel, for access to the error classes
     * @param code The program to compile.  Must be NUL-terminated; the NUL is not considered
     *             part of the input.  Will be converted to UTF-16 if it is not in that format.
     * @param filename The name of the input file if the input came from a file, otherwise this
     *                 /must/ be NULL.  If the input came from a file then the 'include' directive
     *                 will be honored (recursively), otherwise not.
     * @return The ScriptBuffer containing the code.
     * @exception Throws 'SyntaxError' as appropriate. Throws 'Error' on internal errors (won't
     *            normally happen).
     */
    extern ScriptBuffer compileProgram(AvmCore* core, Toplevel* toplevel, String* code, String* filename);
}

#endif // VMCFG_EVAL
