// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component workers
%%category NoSyncSingleItemBuffer

// Bugzilla: https://bugzilla.mozilla.org/show_bug.cgi?id=758260
%%ifndef DEBUGGER

%%methods

%%decls
    MMgc::GC *gc;
	NoSyncSingleItemBuffer *buffer;
    Atom *atom;
 
%%prologue
    gc=core->GetGC();
    atom=(Atom *)malloc(100);
    buffer=new NoSyncSingleItemBuffer(gc);

%%epilogue
	delete buffer;
    free(atom);
	
%%test constructor
    %%verify buffer!=NULL
	
%%test emptyBuffer
    %%verify buffer->isEmpty()
	%%verify !buffer->isFull()
	%%verify !buffer->get(atom)
	
%%test fullBuffer
    buffer->put(String::createLatin1(core,"test string")->atom());
	%%verify !buffer->isEmpty()
	%%verify buffer->isFull()
	%%verify buffer->get(atom)
	%%verify buffer->isEmpty()
	%%verify !buffer->isFull()
	
%%test destroy
    buffer->unregisterRoot();
	%%verify true

	
