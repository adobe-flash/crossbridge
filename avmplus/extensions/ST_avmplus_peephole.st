// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component avmplus
%%category  peephole
%%ifdef     AVMPLUS_PEEPHOLE_OPTIMIZER

%%decls

private:
#ifdef AVMPLUS_DIRECT_THREADED
    void** opcode_labels; // the name is not arbitrary
#endif

%%prologue

#ifdef AVMPLUS_DIRECT_THREADED
    opcode_labels = interpGetOpcodeLabels();
#endif

%%epilogue

#ifdef AVMPLUS_DIRECT_THREADED
    opcode_labels = NULL; // interpGetOpcodeLables() returns a pointer to static data
#endif

%%test get2locals

    WordcodeEmitter* t = new WordcodeEmitter(core, NULL);

     t->emitOp1(WOP_getlocal, 5);
     t->emitOp1(WOP_getlocal, 4);
     t->emitOp1(WOP_getlocal, 65536);
     t->emitOp1(WOP_getlocal, 7);
     t->emitOp1(WOP_getlocal, 6);
     uintptr_t* code;
uint32_t len = (uint32_t)t->epilogue(&code);

%%verify len == 6
%%verify code[0] == NEW_OPCODE(WOP_get2locals)
%%verify code[1] == ((4 << 16) | 5)
%%verify code[2] == NEW_OPCODE(WOP_getlocal)
%%verify code[3] == 65536
%%verify code[4] == NEW_OPCODE(WOP_get2locals)
%%verify code[5] == ((6 << 16) | 7)

    delete t;

