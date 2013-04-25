// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component workers
%%category Buffers

// Bugzilla: https://bugzilla.mozilla.org/show_bug.cgi?id=758260
%%ifndef DEBUGGER

%%decls
    MMgc::GC *gc;
    NoSyncSingleItemBuffer *singleItemBuffer;
    NoSyncMultiItemBuffer *multiItemBuffer;
    NonBlockingMultiItemBuffer *nonBlockMultiItemBuffer;
    Atom *atom;
 
%%prologue
    MMgc::GCConfig config;
    gc = new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
	//gc=core->GetGC();
    atom=(Atom *)malloc(100);
    singleItemBuffer=new NoSyncSingleItemBuffer(gc);
    multiItemBuffer=new NoSyncMultiItemBuffer();
    nonBlockMultiItemBuffer=new NonBlockingMultiItemBuffer(gc);

%%epilogue
    delete singleItemBuffer;
    delete multiItemBuffer;
    delete nonBlockMultiItemBuffer;
	delete gc;
    free(atom);

%%test NonBlockMultiItemConstructor
    %%verify nonBlockMultiItemBuffer!=NULL

%%test NonBlockMultiItemEmptyBuffer
    %%verify nonBlockMultiItemBuffer->isEmpty()
    %%verify !nonBlockMultiItemBuffer->isFull()
    %%verify !nonBlockMultiItemBuffer->get(atom)

%%test NonBlockMultiItemFullBuffer
    bool result;
    for (int i=0;i<14;i++) {
        result=nonBlockMultiItemBuffer->put(String::createLatin1(core,"test string")->atom());
        %%verify result
        %%verify !nonBlockMultiItemBuffer->isEmpty()
        %%verify !nonBlockMultiItemBuffer->isFull()
    }
    result=nonBlockMultiItemBuffer->put(String::createLatin1(core,"test string")->atom());
    %%verify result
    %%verify !nonBlockMultiItemBuffer->isEmpty()
    %%verify nonBlockMultiItemBuffer->isFull()

    result=nonBlockMultiItemBuffer->put(String::createLatin1(core,"test string")->atom());
    %%verify !result
    %%verify !nonBlockMultiItemBuffer->isEmpty()
    %%verify nonBlockMultiItemBuffer->isFull()

    for (int i=0;i<14;i++) {
        %%verify nonBlockMultiItemBuffer->get(atom)
        %%verify !nonBlockMultiItemBuffer->isEmpty()
        %%verify !nonBlockMultiItemBuffer->isFull()
    }
    %%verify nonBlockMultiItemBuffer->get(atom)
    %%verify nonBlockMultiItemBuffer->isEmpty()
    %%verify !nonBlockMultiItemBuffer->isFull()

    %%verify !nonBlockMultiItemBuffer->get(atom)
    %%verify nonBlockMultiItemBuffer->isEmpty()
    %%verify !nonBlockMultiItemBuffer->isFull()

%%test SingleConstructor
    %%verify singleItemBuffer!=NULL

%%test SingleEmptyBuffer
    %%verify singleItemBuffer->isEmpty()
    %%verify !singleItemBuffer->isFull()
    %%verify !singleItemBuffer->get(atom)

%%test SingleFullBuffer
    bool result;
    result=singleItemBuffer->put(String::createLatin1(core,"test string")->atom());
    %%verify result
    %%verify !singleItemBuffer->isEmpty()
    %%verify singleItemBuffer->isFull()
    result=singleItemBuffer->put(String::createLatin1(core,"test string")->atom());
    %%verify !result
    %%verify singleItemBuffer->get(atom)
    %%verify singleItemBuffer->isEmpty()
    %%verify !singleItemBuffer->isFull()

%%test MultiConstructor
    %%verify multiItemBuffer!=NULL

%%test MultiEmptyBuffer
    %%verify singleItemBuffer->isEmpty()
    %%verify !singleItemBuffer->isFull()
    %%verify !singleItemBuffer->get(atom)

// Bugzilla: https://bugzilla.mozilla.org/show_bug.cgi?id=758260
//%%test MultiFullBuffer
//    bool result;
//    for (int i=0;i<15;i++) {
//        ChannelItem item;
//        item.tag = kDoubleType;
//        item.asNumber = 3.14;
//        result=multiItemBuffer->put(item);
//        %%verify result
//        %%verify !multiItemBuffer->isEmpty()
//        %%verify !multiItemBuffer->isFull()
//    }
//    ChannelItem inItem;
//    inItem.asNumber = 3.14;
//    inItem.tag = kDoubleType;
//    result=multiItemBuffer->put(inItem);
//    %%verify result
//    %%verify !multiItemBuffer->isEmpty()
//    %%verify multiItemBuffer->isFull()
//
//    result=multiItemBuffer->put(inItem);
//    %%verify !result
//    %%verify !multiItemBuffer->isEmpty()
//    %%verify multiItemBuffer->isFull()
//

