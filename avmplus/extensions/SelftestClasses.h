/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

class MyObject {
public:
    int i;
};
class MyGCObject : public MMgc::GCObject {
public:
    int i;
    MyGCObject *child;
    MyGCObject() {
        child=NULL;
    }
};
class MyGCLargeObject : public MMgc::GCObject {
public:
    char array[MMgc::GCHeap::kBlockSize];
    MyGCLargeObject() {}  
};

