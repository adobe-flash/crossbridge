// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 543560 - here we risk deleting an object that is still on the mark stack because
// of how we perform large-object splitting.  The setup is that user code that deletes the object
// gets to run after the first part of the large object has been popped off the mark stack
// but before the rest has been handled.

%%component mmgc
%%category bugzilla_543560
%%ifdef     AVMPLUS_WIN32
%%ifndef    VMCFG_ARM

%%methods
using namespace MMgc;

// allocate a bunch big things
// allocate a bunch of small things  ( to pump incremental mark.... )
// explicitly free one of the big things
// ... crash

struct BigThing;

static inline unsigned getSerial()
{
    static unsigned g_counter = 0;
    unsigned result = g_counter;
    ++g_counter;
    return result;
}

struct BigThing : public MMgc::GCFinalizedObject
{
    BigThing() : m_next(0), m_prev(0), m_serial(getSerial())
    {
        VMPI_memset(&m_data, 0, sizeof(m_data));
    }

    virtual ~BigThing()
    {
        //printf("~BigThing: %u 0x%08X 0x%08X\n", m_serial, this, this + 1);
    }
    GCMember<BigThing> m_next;
    GCMember<BigThing> m_prev;
    unsigned m_serial;
    char m_data[512 * 1024];
};

BigThing* makeBigThings(MMgc::GC* gc, size_t howMany)
{
    BigThing* first = 0;
    BigThing* curr = 0;
    for (unsigned i = 0; i < howMany; ++i) {
        BigThing* newThing = new (gc) BigThing();
        if (!first) {
            first = newThing;
        }
        else {
            curr->m_next = newThing;
            newThing->m_prev = curr;
        }
        curr = newThing;
    }
    return first;
}

struct SmallThing : public MMgc::GCFinalizedObject
{
    char m_data[200];
    virtual ~SmallThing()
    {
    }
};

struct MyRoot : public MMgc::GCRoot
{
    MyRoot(MMgc::GC* gc) : MMgc::GCRoot(gc) {}
    BigThing* m_bigThings;
};

%%decls
private:
    MMgc::GC *gc;

%%prologue
    MMgc::GCConfig config;
    gc = new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);

%%epilogue
    delete gc;

%%test bugzilla_543560
    MMGC_GCENTER(gc);

    MyRoot* theRoot = 0;
    {
        BigThing* volatile bigThings = makeBigThings(gc, 400);
        theRoot = new MyRoot(gc);
        theRoot->m_bigThings = bigThings;
    }
    BigThing* volatile middle = theRoot->m_bigThings;
    for (int j = 0 ; j < 150; ++j)
        middle = middle->m_next;

    middle->m_prev->m_next = 0;
    for (int j = 0; j < 50; ++j)
        middle = middle->m_next;

    middle->m_prev = 0;
    gc->Collect();
    gc->Collect();

    for (int j = 0; j < 100000; ++j) {
        //printf("j: %d\n", j);
        for (int i = 0; i < 500; ++i) {
            new (gc) SmallThing();
            if ((theRoot->m_bigThings) && (MMgc::GC::GetMark(theRoot->m_bigThings))) {
                while (theRoot->m_bigThings != 0) {
                    BigThing* curr = theRoot->m_bigThings;
                    theRoot->m_bigThings = theRoot->m_bigThings->m_next;
                    delete curr;
                }
                MMgc::GCHeap::GetGCHeap()->Decommit();
            }
        }

    }
    delete theRoot;

    // Will crash if it fails so the %%verify is just token
    %%verify true
