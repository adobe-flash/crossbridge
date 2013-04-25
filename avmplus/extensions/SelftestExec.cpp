// Generated from ST_avmplus_basics.st, ST_avmplus_builtins.st, ST_avmplus_peephole.st, ST_avmplus_vector_accessors.st, ST_mmgc_543560.st, ST_mmgc_575631.st, ST_mmgc_580603.st, ST_mmgc_603411.st, ST_mmgc_637993.st, ST_mmgc_basics.st, ST_mmgc_dependent.st, ST_mmgc_exact.st, ST_mmgc_externalalloc.st, ST_mmgc_finalize_uninit.st, ST_mmgc_fixedmalloc_findbeginning.st, ST_mmgc_gcheap.st, ST_mmgc_gcoption.st, ST_mmgc_mmfx_array.st, ST_mmgc_threads.st, ST_mmgc_weakref.st, ST_nanojit_codealloc.st, ST_vmbase_concurrency.st, ST_vmbase_safepoints.st, ST_vmpi_threads.st, ST_workers_Buffer.st, ST_workers_NoSyncSingleItemBuffer.st, ST_workers_Promise.st
// Generated from ST_avmplus_basics.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_avmplus_basics {
class ST_avmplus_basics : public Selftest {
public:
ST_avmplus_basics(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
};
ST_avmplus_basics::ST_avmplus_basics(AvmCore* core)
    : Selftest(core, "avmplus", "basics", ST_avmplus_basics::ST_names,ST_avmplus_basics::ST_explicits)
{}
const char* ST_avmplus_basics::ST_names[] = {"unsigned_int","signed_int","equalsLatin1","containsLatin1","indexOfLatin1","matchesLatin1","matchesLatin1_caseless","bug562101", NULL };
const bool ST_avmplus_basics::ST_explicits[] = {false,false,false,false,false,false,false,false, false };
void ST_avmplus_basics::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
case 5: test5(); return;
case 6: test6(); return;
case 7: test7(); return;
}
}
void ST_avmplus_basics::test0() {

// Does right shift of unsigned quantities work?
// line 14 "ST_avmplus_basics.st"
verifyPass((int)(~0U >> 1) > 0, "(int)(~0U >> 1) > 0", __FILE__, __LINE__);

}
void ST_avmplus_basics::test1() {

// Does right shift of signed quantities work?
// line 19 "ST_avmplus_basics.st"
verifyPass((-1 >> 1) == -1, "(-1 >> 1) == -1", __FILE__, __LINE__);

// verify that the "latin1" literal string calls work properly for hi-bit latin1 chars
}
void ST_avmplus_basics::test2() {
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool equals = s->equalsLatin1("ev\xADident");
// line 25 "ST_avmplus_basics.st"
verifyPass(equals == true, "equals == true", __FILE__, __LINE__);

}
void ST_avmplus_basics::test3() {
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool found = s->containsLatin1("\xAD");
// line 30 "ST_avmplus_basics.st"
verifyPass(found == true, "found == true", __FILE__, __LINE__);

}
void ST_avmplus_basics::test4() {
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    int index = s->indexOfLatin1("\xAD");
// line 35 "ST_avmplus_basics.st"
verifyPass(index == 2, "index == 2", __FILE__, __LINE__);

}
void ST_avmplus_basics::test5() {
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool matches1 = s->matchesLatin1("\xADi", 2, 2);
// line 40 "ST_avmplus_basics.st"
verifyPass(matches1 == true, "matches1 == true", __FILE__, __LINE__);

}
void ST_avmplus_basics::test6() {
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool matches2 = s->matchesLatin1_caseless("\xADIDENT", 2, 2);
// line 45 "ST_avmplus_basics.st"
verifyPass(matches2 == true, "matches2 == true", __FILE__, __LINE__);


}
void ST_avmplus_basics::test7() {
// XMLParser omits the last char of a DOCTYPE node
Stringp str = core->newConstantStringLatin1("<?xml version=\"1.0\"?><!DOCTYPE greeting SYSTEM><greeting>Hello, world!</greeting>");
XMLParser parser(core, str);
MMgc::GC *gc = core->GetGC();
XMLTag tag(gc);
int m_status;
bool pass = false;
while ((m_status = parser.getNext(tag)) == XMLParser::kNoError)
{
    switch (tag.nodeType)
    {
        case XMLTag::kDocTypeDeclaration:
        {
            pass = false;
            pass = tag.text->equalsLatin1("<!DOCTYPE greeting SYSTEM>");
        }
    break;
    }
}
// line 68 "ST_avmplus_basics.st"
verifyPass(pass == true, "pass == true", __FILE__, __LINE__);

    // FIXME: this needs a "register this object with the GC" mechanism; this abuse of the GCRoot mechanism
    // is no longer allowed
//%%test bug610022
//    Stringp str = core->newConstantStringLatin1("some string that is likely to be unique");
//    WeakRefList<String> list(core->GetGC(), 0);
    // We are going to skip scanning the stack (so that "str" won't hold the string in place)
    // but that means we need a root to ensure that "list" doesn't also get collected.
    //
    //MMgc::GCRoot root(core->GetGC(), &list, sizeof(list));
    //list.add(str);
    //str = NULL;
    //core->GetGC()->Collect(/*scanStack*/false);
    //int removed = list.removeCollectedItems();
    //int count = list.length();
//%%verify removed == 1 && count == 0



}
void create_avmplus_basics(AvmCore* core) { new ST_avmplus_basics(core); }
}
}
#endif

// Generated from ST_avmplus_builtins.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_avmplus_builtins {
class ST_avmplus_builtins : public Selftest {
public:
ST_avmplus_builtins(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
};
ST_avmplus_builtins::ST_avmplus_builtins(AvmCore* core)
    : Selftest(core, "avmplus", "builtins", ST_avmplus_builtins::ST_names,ST_avmplus_builtins::ST_explicits)
{}
const char* ST_avmplus_builtins::ST_names[] = {"WeakValueHashTable", NULL };
const bool ST_avmplus_builtins::ST_explicits[] = {false, false };
void ST_avmplus_builtins::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_avmplus_builtins::test0() {
    WeakValueHashtable* tbl = WeakValueHashtable::create(core->gc);
    String* fhtagn = String::createLatin1(core, "Fhtagn!");
    MMgc::GCObjectLock* fhtagn_lock = core->gc->LockObject(fhtagn);
    fhtagn = NULL;
    for ( int i=0 ; i < 500 ; i++ ) {
        tbl->add(atomFromIntptrValue(i), (i & 1) ? String::createLatin1(core, "Cthulhu!")->atom() : ((String*)core->gc->GetLockedObject(fhtagn_lock))->atom());
    }
    core->gc->Collect();
    core->gc->Collect();
    int sum = 0;
    for ( int i=0 ; i < 500 ; i++ ) {
        Atom a = tbl->get(atomFromIntptrValue(i));
        if (a != AtomConstants::undefinedAtom)
            sum++;
    }
    core->gc->UnlockObject(fhtagn_lock);
    printf("fhtagn sum: %d\n", sum);

// Retain at least 250, but it would be unreasonable to retain more than 300
// line 31 "ST_avmplus_builtins.st"
verifyPass(sum >= 250 && sum <= 300, "sum >= 250 && sum <= 300", __FILE__, __LINE__);

}
void create_avmplus_builtins(AvmCore* core) { new ST_avmplus_builtins(core); }
}
}
#endif

// Generated from ST_avmplus_peephole.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if defined AVMPLUS_PEEPHOLE_OPTIMIZER
namespace avmplus {
namespace ST_avmplus_peephole {
class ST_avmplus_peephole : public Selftest {
public:
ST_avmplus_peephole(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();

private:
#ifdef AVMPLUS_DIRECT_THREADED
    void** opcode_labels; // the name is not arbitrary
#endif

};
ST_avmplus_peephole::ST_avmplus_peephole(AvmCore* core)
    : Selftest(core, "avmplus", "peephole", ST_avmplus_peephole::ST_names,ST_avmplus_peephole::ST_explicits)
{}
const char* ST_avmplus_peephole::ST_names[] = {"get2locals", NULL };
const bool ST_avmplus_peephole::ST_explicits[] = {false, false };
void ST_avmplus_peephole::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_avmplus_peephole::prologue() {

#ifdef AVMPLUS_DIRECT_THREADED
    opcode_labels = interpGetOpcodeLabels();
#endif

}
void ST_avmplus_peephole::epilogue() {

#ifdef AVMPLUS_DIRECT_THREADED
    opcode_labels = NULL; // interpGetOpcodeLables() returns a pointer to static data
#endif

}
void ST_avmplus_peephole::test0() {

    WordcodeEmitter* t = new WordcodeEmitter(core, NULL);

     t->emitOp1(WOP_getlocal, 5);
     t->emitOp1(WOP_getlocal, 4);
     t->emitOp1(WOP_getlocal, 65536);
     t->emitOp1(WOP_getlocal, 7);
     t->emitOp1(WOP_getlocal, 6);
     uintptr_t* code;
uint32_t len = (uint32_t)t->epilogue(&code);

// line 43 "ST_avmplus_peephole.st"
verifyPass(len == 6, "len == 6", __FILE__, __LINE__);
// line 44 "ST_avmplus_peephole.st"
verifyPass(code[0] == NEW_OPCODE(WOP_get2locals), "code[0] == NEW_OPCODE(WOP_get2locals)", __FILE__, __LINE__);
// line 45 "ST_avmplus_peephole.st"
verifyPass(code[1] == ((4 << 16) | 5), "code[1] == ((4 << 16) | 5)", __FILE__, __LINE__);
// line 46 "ST_avmplus_peephole.st"
verifyPass(code[2] == NEW_OPCODE(WOP_getlocal), "code[2] == NEW_OPCODE(WOP_getlocal)", __FILE__, __LINE__);
// line 47 "ST_avmplus_peephole.st"
verifyPass(code[3] == 65536, "code[3] == 65536", __FILE__, __LINE__);
// line 48 "ST_avmplus_peephole.st"
verifyPass(code[4] == NEW_OPCODE(WOP_get2locals), "code[4] == NEW_OPCODE(WOP_get2locals)", __FILE__, __LINE__);
// line 49 "ST_avmplus_peephole.st"
verifyPass(code[5] == ((6 << 16) | 7), "code[5] == ((6 << 16) | 7)", __FILE__, __LINE__);

    delete t;


}
void create_avmplus_peephole(AvmCore* core) { new ST_avmplus_peephole(core); }
}
}
#endif
#endif

// Generated from ST_avmplus_vector_accessors.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 609145 - VectorObject needs fast inline getter/setters
// Make sure the APIs, which are used by the Flash Player and AIR only, do not disappear.
//
// NOTE, the following comment is stale and we can fix the code, see the code for
// VectorAccessor further down for how to access a toplevel.
//
// We can't test them because we don't have access to a Toplevel*, but we can reference
// them, and a link error will ensue if they disappear.
//
// Code coverage will get worse with this test, not better.  C'est la guerre.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_avmplus_vector_accessors {

int32_t reference_getUintPropertyFast(IntVectorObject* obj, uint32_t x)
{
    return obj->getUintPropertyFast(x);
}

void reference_setUintPropertyFast(IntVectorObject* obj, uint32_t x, int32_t v)
{
    obj->setUintPropertyFast(x, v);
}

class ST_avmplus_vector_accessors : public Selftest {
public:
ST_avmplus_vector_accessors(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
};
ST_avmplus_vector_accessors::ST_avmplus_vector_accessors(AvmCore* core)
    : Selftest(core, "avmplus", "vector_accessors", ST_avmplus_vector_accessors::ST_names,ST_avmplus_vector_accessors::ST_explicits)
{}
const char* ST_avmplus_vector_accessors::ST_names[] = {"getOrSetUintPropertyFast","DataListAccessor_on_int","DataListAccessor_on_float4","VectorAccessor_on_int","VectorAccessor_on_float4", NULL };
const bool ST_avmplus_vector_accessors::ST_explicits[] = {false,false,false,false,false, false };
void ST_avmplus_vector_accessors::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
}
}
void ST_avmplus_vector_accessors::test0() {

// line 36 "ST_avmplus_vector_accessors.st"
verifyPass(true, "true", __FILE__, __LINE__);

}
void ST_avmplus_vector_accessors::test1() {

DataList<int> dl(core->GetGC(), 3);
dl.add(1);
dl.add(1);
dl.add(2);
dl.add(3);
dl.add(5);
dl.add(8);
dl.add(13);
DataListAccessor<int> dla(&dl);
int* xs = dla.addr();

// line 51 "ST_avmplus_vector_accessors.st"
verifyPass(xs[0] == 1, "xs[0] == 1", __FILE__, __LINE__);
// line 52 "ST_avmplus_vector_accessors.st"
verifyPass(xs[1] == 1, "xs[1] == 1", __FILE__, __LINE__);
// line 53 "ST_avmplus_vector_accessors.st"
verifyPass(xs[2] == 2, "xs[2] == 2", __FILE__, __LINE__);
// line 54 "ST_avmplus_vector_accessors.st"
verifyPass(xs[3] == 3, "xs[3] == 3", __FILE__, __LINE__);
// line 55 "ST_avmplus_vector_accessors.st"
verifyPass(xs[4] == 5, "xs[4] == 5", __FILE__, __LINE__);
// line 56 "ST_avmplus_vector_accessors.st"
verifyPass(xs[5] == 8, "xs[5] == 8", __FILE__, __LINE__);
// line 57 "ST_avmplus_vector_accessors.st"
verifyPass(xs[6] == 13, "xs[6] == 13", __FILE__, __LINE__);

}
void ST_avmplus_vector_accessors::test2() {

#ifdef VMCFG_FLOAT

DataList<float4_t, 16> dl4(core->GetGC(), 3);
float4_t x0 = { 1,1,2,3 };
float4_t x1 = { 5,8,13,21 };
float4_t x2 = { 34,55,89,144 };
dl4.add(x0);
dl4.add(x1);
dl4.add(x2);
DataListAccessor<float4_t,16> dla4(&dl4);
float4_t* x4s = dla4.addr();

// line 73 "ST_avmplus_vector_accessors.st"
verifyPass(f4_eq_i(x4s[0], x0) == 1, "f4_eq_i(x4s[0], x0) == 1", __FILE__, __LINE__);
// line 74 "ST_avmplus_vector_accessors.st"
verifyPass(f4_eq_i(x4s[1], x1) == 1, "f4_eq_i(x4s[1], x1) == 1", __FILE__, __LINE__);
// line 75 "ST_avmplus_vector_accessors.st"
verifyPass(f4_eq_i(x4s[2], x2) == 1, "f4_eq_i(x4s[2], x2) == 1", __FILE__, __LINE__);

#else

// line 79 "ST_avmplus_vector_accessors.st"
verifyPass(true, "true", __FILE__, __LINE__);

#endif

}
void ST_avmplus_vector_accessors::test3() {

#ifdef AVMSHELL_BUILD

avmshell::ShellCore* c = (avmshell::ShellCore*)core;
avmshell::ShellToplevel* top = c->shell_toplevel;
IntVectorObject* vec = top->intVectorClass()->newVector();

vec->_setNativeUintProperty(0, 1);
vec->_setNativeUintProperty(1, 1);
vec->_setNativeUintProperty(2, 2);
vec->_setNativeUintProperty(3, 3);
vec->_setNativeUintProperty(4, 5);
vec->_setNativeUintProperty(5, 8);
vec->_setNativeUintProperty(6, 13);

IntVectorAccessor va(vec);
int* xs = va.addr();

// line 102 "ST_avmplus_vector_accessors.st"
verifyPass(xs[0] == 1, "xs[0] == 1", __FILE__, __LINE__);
// line 103 "ST_avmplus_vector_accessors.st"
verifyPass(xs[1] == 1, "xs[1] == 1", __FILE__, __LINE__);
// line 104 "ST_avmplus_vector_accessors.st"
verifyPass(xs[2] == 2, "xs[2] == 2", __FILE__, __LINE__);
// line 105 "ST_avmplus_vector_accessors.st"
verifyPass(xs[3] == 3, "xs[3] == 3", __FILE__, __LINE__);
// line 106 "ST_avmplus_vector_accessors.st"
verifyPass(xs[4] == 5, "xs[4] == 5", __FILE__, __LINE__);
// line 107 "ST_avmplus_vector_accessors.st"
verifyPass(xs[5] == 8, "xs[5] == 8", __FILE__, __LINE__);
// line 108 "ST_avmplus_vector_accessors.st"
verifyPass(xs[6] == 13, "xs[6] == 13", __FILE__, __LINE__);

#else

// line 112 "ST_avmplus_vector_accessors.st"
verifyPass(true, "true", __FILE__, __LINE__);

#endif // AVMSHELL_BUILD

}
void ST_avmplus_vector_accessors::test4() {

#if defined VMCFG_FLOAT && defined AVMSHELL_BUILD

avmshell::ShellCore* c = (avmshell::ShellCore*)core;
avmshell::ShellToplevel* top = c->shell_toplevel;
Float4VectorObject* vec = top->float4VectorClass()->newVector();

float4_t x0 = { 1,1,2,3 };
float4_t x1 = { 5,8,13,21 };
float4_t x2 = { 34,55,89,144 };

vec->_setNativeUintProperty(0, x0);
vec->_setNativeUintProperty(1, x1);
vec->_setNativeUintProperty(2, x2);

Float4VectorAccessor va(vec);
float4_t* x4s = va.addr();

// line 135 "ST_avmplus_vector_accessors.st"
verifyPass(f4_eq_i(x4s[0], x0) == 1, "f4_eq_i(x4s[0], x0) == 1", __FILE__, __LINE__);
// line 136 "ST_avmplus_vector_accessors.st"
verifyPass(f4_eq_i(x4s[1], x1) == 1, "f4_eq_i(x4s[1], x1) == 1", __FILE__, __LINE__);
// line 137 "ST_avmplus_vector_accessors.st"
verifyPass(f4_eq_i(x4s[2], x2) == 1, "f4_eq_i(x4s[2], x2) == 1", __FILE__, __LINE__);

#else

// line 141 "ST_avmplus_vector_accessors.st"
verifyPass(true, "true", __FILE__, __LINE__);

#endif // AVMSHELL_BUILD

}
void create_avmplus_vector_accessors(AvmCore* core) { new ST_avmplus_vector_accessors(core); }
}
}
#endif

// Generated from ST_mmgc_543560.st
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

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if defined AVMPLUS_WIN32
#if !defined VMCFG_ARM
namespace avmplus {
namespace ST_mmgc_bugzilla_543560 {
class ST_mmgc_bugzilla_543560 : public Selftest {
public:
ST_mmgc_bugzilla_543560(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
private:
    MMgc::GC *gc;

};
ST_mmgc_bugzilla_543560::ST_mmgc_bugzilla_543560(AvmCore* core)
    : Selftest(core, "mmgc", "bugzilla_543560", ST_mmgc_bugzilla_543560::ST_names,ST_mmgc_bugzilla_543560::ST_explicits)
{}
const char* ST_mmgc_bugzilla_543560::ST_names[] = {"bugzilla_543560", NULL };
const bool ST_mmgc_bugzilla_543560::ST_explicits[] = {false, false };
void ST_mmgc_bugzilla_543560::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_mmgc_bugzilla_543560::prologue() {
    MMgc::GCConfig config;
    gc = new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);

}
void ST_mmgc_bugzilla_543560::epilogue() {
    delete gc;

}
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

void ST_mmgc_bugzilla_543560::test0() {
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
// line 135 "ST_mmgc_543560.st"
verifyPass(true, "true", __FILE__, __LINE__);

}
void create_mmgc_bugzilla_543560(AvmCore* core) { new ST_mmgc_bugzilla_543560(core); }
}
}
#endif
#endif
#endif

// Generated from ST_mmgc_575631.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 565631 - We occasionally interleave invoking finalizers
// and clearing mark bits in GCAlloc::Finalize; so a finalizer can
// observe a live object that does not have its mark bit set.
//
// This complicates things because we want to ensure that unmarked
// weakly-referenced objects are resurrected by the GC if the weak
// reference is dereferenced during presweep, but we do not want to
// schedule collection work (or set bits that are supposed to be
// unmarked) during finalization.
//
// (Long term we might want to get rid of the interleaving of
// finalization and mark-bit clearing.  Short term, lets just
// try to detect this on our own.)

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_bugzilla_575631 {
using namespace MMgc;

// Upon destruction, start reading weak refs of "friends" near and far
class Snoopy : public GCFinalizedObject
{
public:
    Snoopy(int key, GCWeakRef** refs, int len)
        : key(key), friends(refs), len(len)
    {
        ++alive_count;
    }
    ~Snoopy();
    static int alive_count;
private:
    int key;
    GCWeakRef** friends;
    int len;
};

// To take D samples from an array of N elems, walk thru by floor(N/D)
// steps (but avoid the pathological case when the floor is zero).
int compute_stride(int numerator, int denominator)
{
    int delta = numerator / denominator;
    return (delta > 0) ? delta : 1;
}

class ST_mmgc_bugzilla_575631 : public Selftest {
public:
ST_mmgc_bugzilla_575631(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
// collecting twice is only "sure" way to gc in presence of incrementality
void collect2() { core->gc->Collect(); core->gc->Collect(); }

};
ST_mmgc_bugzilla_575631::ST_mmgc_bugzilla_575631(AvmCore* core)
    : Selftest(core, "mmgc", "bugzilla_575631", ST_mmgc_bugzilla_575631::ST_names,ST_mmgc_bugzilla_575631::ST_explicits)
{}
const char* ST_mmgc_bugzilla_575631::ST_names[] = {"drizzle", NULL };
const bool ST_mmgc_bugzilla_575631::ST_explicits[] = {false, false };
void ST_mmgc_bugzilla_575631::run(int n) {
switch(n) {
case 0: test0(); return;
}
}

/*static*/ int Snoopy::alive_count = 0;

const int arr_len = 1000;
const int lookups_per_destruct = 10;
const int destructs = 10;
Snoopy::~Snoopy()
{
    int delta = compute_stride(arr_len, lookups_per_destruct);

    for ( int i = 1 ; i < arr_len ; i += delta ) {
        int idx = (key + i) % len;
        // printf("referencing ref[%d] from Snoopy(%d)\n", idx, key);
        friends[idx]->get();
    }
    --alive_count;
}

void ST_mmgc_bugzilla_575631::test0() {
{
    GC* gc = core->gc;

    Snoopy* objs[arr_len];
    GCWeakRef* refs[arr_len];

    // initial setup:
    for (int i=0 ; i < arr_len; ++i ) {
        objs[i] = new (gc) Snoopy(i, refs, arr_len);
        refs[i] = objs[i]->GetWeakRef();
    }

    collect2();

    int delta = compute_stride(arr_len, destructs);

    for (int i=0; i < arr_len; i += delta) {
        objs[i] = NULL;
        collect2();
    }

    // not assert failing within get() is passing the test.
// line 99 "ST_mmgc_575631.st"
verifyPass(1, "1", __FILE__, __LINE__);
          ; // (make my auto-indenter happy)

    // cleanup code; letting ~Snoopy occur outside test extent is big no-no.
    {
        for (int i=0; i < arr_len; ++i ) {
            if (! refs[i]->isNull())
                delete objs[i];
        }

        // if something went wrong above and some Snoopy's are still alive,
        // we'll get burned during their destructors.  Make sure that
        // does not happen.
// line 112 "ST_mmgc_575631.st"
verifyPass((Snoopy::alive_count == 0), "(Snoopy::alive_count == 0)", __FILE__, __LINE__);
              ;
    }
}

}
void create_mmgc_bugzilla_575631(AvmCore* core) { new ST_mmgc_bugzilla_575631(core); }
}
}
#endif

// Generated from ST_mmgc_580603.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 580603 - adversarial mmgc: dispersive w.r.t. address space
// Bugzilla 445780 - Page map needs to be sparse
// 
// Tommy: "problems in 64-bit linux b/c VMPI_allocateAlignedMemory provides
// memory from low 32 bit-addresses, and mmap is giving us addresses from
// 0x7fxxxxxxxxxxxxxx so we need a 4 GB page map."
//
// This self-test attempts to replicate the problem described above
// (by coordinating with an adversarial gcheap) thus illustrating the
// need for a sparse page map.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if defined AVMPLUS_64BIT && defined DEBUG
namespace avmplus {
namespace ST_mmgc_bugzilla_580603 {
using namespace MMgc;

class ST_mmgc_bugzilla_580603 : public Selftest {
public:
ST_mmgc_bugzilla_580603(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();

GC *gc;
GCHeap *heap;

size_t saved_dispersiveAdversarial;
size_t saved_heapLimit;

char *min_addr;
char *max_start;
char *max_addr;
size_t min_size;
size_t max_size;

static const int m_num_sizes = 17;
static const int m_num_iters =  2;
static const int m_init_size = 16;
static const int m_dfactor   =  2;

void print_minmax()
{
    intptr_t delta = max_addr - min_addr;
    intptr_t v = delta;
    unsigned r = 0; // r will be floor(log_2(v))

    while (v >>= 1)
        r++;

    printf("min_size: 0x%15lx max_size: 0x%15lx\n",
           (unsigned long)min_size, (unsigned long)max_size);
    printf("min_addr: 0x%15lx max_addr: 0x%15lx delta:% 15ld (>= 2^%u)\n",
           (unsigned long)min_addr, (unsigned long)max_addr, delta, r);
    fflush(NULL);
}

enum alloc_method_t { via_gc, via_heap };

const char* method_string(alloc_method_t m)
{
    switch (m) {
    case via_gc:     return "via_gc";
    case via_heap:   return "via_heap";
    default: return 0;
    }
}

char *my_alloc(size_t size, alloc_method_t m)
{
    char *ret;

    // Took heap-zeroing and heap-profiling out of flags;
    // avoid wasting time mapping in memory to initialize to 0.
    int heapFlags = (MMgc::GCHeap::kExpand);
    size_t sizeInPages = (size+(GCHeap::kBlockSize-1))/GCHeap::kBlockSize;

    switch (m) {
    case via_gc:     ret = (char*)gc->Alloc(size); break;
    case via_heap:
        ret = (char*)heap->Alloc(sizeInPages, heapFlags);
        break;
    default: ret = 0; break;
    }

    if (size < min_size)
        min_size = size;
    if (size > max_size)
        max_size = size;
    if (ret < min_addr)
        min_addr = ret;
    if (ret > max_start)
        max_start = ret;
    if (ret+size > max_addr)
        max_addr = ret+size;

    // printf("my_alloc(%10ld, %10s) => %p\n", size, method_string(m), ret);
    // fflush(NULL);
    return ret;
}

void my_free(char *p, alloc_method_t m)
{
    switch (m) {
    case via_gc:     gc->Free(p); return;
    case via_heap:   heap->FreeNoProfile(p); return;
    default: AvmAssert(p == 0); return;
    }
}

// fills recv[] with the addresses of a series of N allocated and
// freed blocks, where N = (m_num_iters * m_num_sizes).
//
// For each iteration:
//
// 1. allocate m_num_sizes blocks of distinct (and exponentially
//    growing) size.
// 2. after finishing the allocations from (1.), free the blocks
//    allocated in the *previous* iteration.
//
// The intention is to allocate blocks covering a wide range of
// sizes in order to tickle the memory subsystem's ability to
// track blocks of different sizes.  Keeping each iteration's
// blocks alive across the next iteration is meant to ensure
// that the set of allocated addresses have many wide gaps,
// stressing the memory subsystem's ability to represent
// the meta-data for disparately allocated ranges.
//
// The loop attempts to keep the block sizes distinct both within any
// particular iteration (the exponential growth) and also across the
// distinct iterations.  This was an artifact of trying to force the
// memory subsystem to be unable to return back blocks that had been
// previously freed.  It may or may not still be necessary with the
// addition of the dispersiveAdversarial gcheap config mode; it does
// not seem to hurt the effectiveness of this test, since one can
// observe OOM failures when using PageMap::Uniform using this test;
// see notes in prologue code below.

void do_allocs(char **recv, alloc_method_t m)
{
    size_t init   = m_init_size;
    int num_iters = m_num_iters;
    int num_sizes = m_num_sizes;
    int dfactor   = m_dfactor;
    for (int j=num_iters-1; j >= 0; j--) {
        for (int i=0, factor=1; i < num_sizes; i++, factor*=dfactor) {
            size_t size = factor*init+num_iters-j;
            // size_t size = factor*init;
            char *result = my_alloc(size, m);
            recv[j*num_sizes+i] = result;
        }
        if (j < num_iters - 1) {
            for (int i=0; i < num_sizes; i++) {
                my_free(recv[(j+1)*num_sizes+i], m);
            }
        }
    }
    for (int i=0; i < num_sizes; i++) {
        my_free(recv[0*num_sizes+i], m);
    }
}


};
ST_mmgc_bugzilla_580603::ST_mmgc_bugzilla_580603(AvmCore* core)
    : Selftest(core, "mmgc", "bugzilla_580603", ST_mmgc_bugzilla_580603::ST_names,ST_mmgc_bugzilla_580603::ST_explicits)
{}
const char* ST_mmgc_bugzilla_580603::ST_names[] = {"alloc_loop_mmgc_viaheap_far","alloc_loop_mmgc_viagc_far", NULL };
const bool ST_mmgc_bugzilla_580603::ST_explicits[] = {false,false, false };
void ST_mmgc_bugzilla_580603::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
}
}
void ST_mmgc_bugzilla_580603::prologue() {
{
    min_addr = (char*)(void*)-1;
    min_size = (size_t)(void*)-1;
    max_addr = 0;
    max_size = 0;

    // based on ST_mmgc_basics.st
    heap = MMgc::GCHeap::GetGCHeap();

    // Constants determining test parameters; see comments below.
    const int k1 = 40;
    const int k2 = 15;

    // filler that GCHeap attempts to insert between allocations,
    // in bytes (not that the precise number matters).
    saved_dispersiveAdversarial = heap->Config().dispersiveAdversarial;
    heap->Config().dispersiveAdversarial = size_t(1) << k1;
    // limit is measured in pages; 1 page = kBlockSize bytes.
    // its default value is absurdly large; cut it down to something
    // where we'll see a failure without first hosing the host machine.
    saved_heapLimit = heap->Config().heapLimit;
    heap->Config().heapLimit = ((1 << k2)-1);

    // Some pairs of (k1,k2) for filler = 2**k1 and limit = 2**k2 - 1,
    // where the (decreasing) value of k2 has just passed threshold to
    // expose OOM from PageMap::Uniform (i.e. limit = 2**(k2+1) - 1
    // won't OOM); test bed is 64-bit DEBUG avmshell on Mac OS X 10.6.4.
    //
    // k1, k2
    // ------
    // 43, 18
    // 42, 17
    // 41, 16
    // 40, 15
    // 39, 14
    // 38, 13
    // 37, 12
    // 36, 12
    // 35, 11
    // 34, 11
    //
    // For k1 < 34, test won't OOM for k2 >= 11.
    // For k2 < 11, test will OOM w/o dispersiveAdversarial at all.

    gc = core->gc;
}

}
void ST_mmgc_bugzilla_580603::epilogue() {
{
    // restore original values to limit disruption to remaining selftests
    heap->Config().heapLimit = this->saved_heapLimit;
    heap->Config().dispersiveAdversarial = saved_dispersiveAdversarial;
}

}

void ST_mmgc_bugzilla_580603::test0() {
{
    char *h[3];
    h[0] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    h[1] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    my_free(h[0], via_heap);
    my_free(h[1], via_heap);
    // print_minmax();

    char *g[m_num_iters*m_num_sizes]; (void)g;
    do_allocs(g, via_heap);

    // print_minmax();

// line 242 "ST_mmgc_580603.st"
verifyPass(1, "1", __FILE__, __LINE__);
}

}
void ST_mmgc_bugzilla_580603::test1() {
{
    char *h[3];
    h[0] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    h[1] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    my_free(h[0], via_heap);
    my_free(h[1], via_heap);
    // print_minmax();

    char *g[m_num_iters*m_num_sizes]; (void)g;
    do_allocs(g, via_gc);

    // print_minmax();

// line 259 "ST_mmgc_580603.st"
verifyPass(1, "1", __FILE__, __LINE__);
}

}
void create_mmgc_bugzilla_580603(AvmCore* core) { new ST_mmgc_bugzilla_580603(core); }
}
}
#endif
#endif

// Generated from ST_mmgc_603411.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bug 603411 - SECURITY: AvmCore dtor needs to clear CallStackNode chain

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if defined DEBUGGER
namespace avmplus {
namespace ST_mmgc_bugzilla_603411 {

class MyDebugger : public Debugger
{
public:
    MyDebugger(AvmCore *core, avmplus::Debugger::TraceLevel tracelevel) : Debugger(core, tracelevel) {}
    ~MyDebugger() {}

    virtual void enterDebugger() {}
    virtual bool filterException(avmplus::Exception*, bool) { return false; }
    virtual bool hitWatchpoint() {return false; }
};


class MyToplevel: public avmplus::Toplevel 
{
    public:
    MyToplevel(avmplus::AbcEnv* env)
        : Toplevel(env) {}
    virtual GCRef<avmplus::ClassClosure> workerClass() const
    {
        return NULL;
    }
    
    virtual GCRef<avmplus::ClassClosure> workerDomainClass() const
    {
        return NULL;
    }

    virtual GCRef<avmplus::ScriptObject> constructWorkerObject() const
    {
        return NULL;
    }
    
};

class MyAvmCore : public avmplus::AvmCore
{
public:
    MyAvmCore(MMgc::GC* gc) : AvmCore(gc, kApiVersionSeries_FP) { initBuiltinPool(1); }
    ~MyAvmCore() {}

    virtual void interrupt(Toplevel*, InterruptReason) { }
    virtual void stackOverflow(Toplevel*) { }
    virtual avmplus::String* readFileForEval(avmplus::String*, avmplus::String*) { return NULL; }
    virtual avmplus::ApiVersion getDefaultAPI() { return kApiVersion_VM_INTERNAL; }
    virtual avmplus::Toplevel* createTopLevel(avmplus::AbcEnv*)
    {
        return NULL;
    }
    Debugger* createDebugger(int tracelevel) { return new (this->gc) MyDebugger(this, (avmplus::Debugger::_TraceLevel)tracelevel); }

    virtual avmplus::Toplevel* createToplevel(avmplus::AbcEnv* env) { 
        return new (gc) MyToplevel(env);
    }

};


class MyTestClass
{
public:
    MyTestClass(MyAvmCore* core) : m_core(core)
    {
        (void)core;
    }

    void testAvmCoreDelete()
    {
        // create CallStackNode
        CallStackNode csn(m_core, "Date");

        // delete core
        delete m_core;
    }
private:
    MyAvmCore* m_core;
};

class ST_mmgc_bugzilla_603411 : public Selftest {
public:
ST_mmgc_bugzilla_603411(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
private:
    MyTestClass* testClass;
    MyAvmCore*   testCore;
};
ST_mmgc_bugzilla_603411::ST_mmgc_bugzilla_603411(AvmCore* core)
    : Selftest(core, "mmgc", "bugzilla_603411", ST_mmgc_bugzilla_603411::ST_names,ST_mmgc_bugzilla_603411::ST_explicits)
{}
const char* ST_mmgc_bugzilla_603411::ST_names[] = {"test1", NULL };
const bool ST_mmgc_bugzilla_603411::ST_explicits[] = {false, false };
void ST_mmgc_bugzilla_603411::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_mmgc_bugzilla_603411::prologue() {

}
void ST_mmgc_bugzilla_603411::epilogue() {
    testClass = NULL;
    testCore = NULL;

}
void ST_mmgc_bugzilla_603411::test0() {
    // create our own core
    testCore = new MyAvmCore(core->gc);
    testClass = new MyTestClass(testCore);
    testClass->testAvmCoreDelete();

// line 108 "ST_mmgc_603411.st"
verifyPass(true, "true", __FILE__, __LINE__);

}
void create_mmgc_bugzilla_603411(AvmCore* core) { new ST_mmgc_bugzilla_603411(core); }
}
}
#endif
#endif

// Generated from ST_mmgc_637993.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 637993: rehashing a GCHashtable in the midst of iteration
// is unsound; here we check that we are guarding against it.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_bugzilla_637993 {
class ST_mmgc_bugzilla_637993 : public Selftest {
public:
ST_mmgc_bugzilla_637993(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();

const static size_t elem_count = 1000;
int32_t *elems;
MMgc::GCHashtable m_table;

void add_first_half() {
    for (size_t i=0; i < elem_count/2; i++)
        m_table.put(&elems[i], &elems[i+1]);
}

void add_second_half() {
    for (size_t i=elem_count/2; i < elem_count; i++)
        m_table.put(&elems[i], &elems[i+1]);
}

};
ST_mmgc_bugzilla_637993::ST_mmgc_bugzilla_637993(AvmCore* core)
    : Selftest(core, "mmgc", "bugzilla_637993", ST_mmgc_bugzilla_637993::ST_names,ST_mmgc_bugzilla_637993::ST_explicits)
{}
const char* ST_mmgc_bugzilla_637993::ST_names[] = {"delete_during_iteration_okay_if_norehash","delete_during_iteration_asserts_if_rehash","rehash_after_iteration_succeeds","rehash_during_iteration_assert_fails_1","rehash_during_iteration_assert_fails_2", NULL };
const bool ST_mmgc_bugzilla_637993::ST_explicits[] = {false,true,false,true,true, false };
void ST_mmgc_bugzilla_637993::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
}
}
void ST_mmgc_bugzilla_637993::test0() {
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        while (it.nextKey()) {
            m_table.remove(it.value(), /*allowrehash=*/false);
        }
    }
    m_table.clear();
    delete elems;
// line 41 "ST_mmgc_637993.st"
verifyPass(true, "true", __FILE__, __LINE__);
         ;

}
void ST_mmgc_bugzilla_637993::test1() {
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        while (it.nextKey()) {
            m_table.remove(it.value());
        }
    }
    m_table.clear();
    delete elems;
// line 55 "ST_mmgc_637993.st"
verifyPass(false, "false", __FILE__, __LINE__);
         ;

// This test is a trivial success; it is meant to be compared against
// the cases that *fail* below, in order to make it clear what is
// wrong with the intentionally asserting cases.
}
void ST_mmgc_bugzilla_637993::test2() {
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        it.nextKey();
        it.nextKey();
    }
    add_second_half(); // rule satisfied; (Iterator is out of scope).
    m_table.clear();
    delete elems;
// line 72 "ST_mmgc_637993.st"
verifyPass(true, "true", __FILE__, __LINE__);
         ;

  // (This test should definitely assert.)
}
void ST_mmgc_bugzilla_637993::test3() {
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        it.nextKey();
        add_second_half(); // this is where we break the rule
        it.nextKey();
    }

    m_table.clear();
    delete elems;
    // we should never get here, the assertion should happen up above.
// line 89 "ST_mmgc_637993.st"
verifyPass(false, "false", __FILE__, __LINE__);
          ;

// This test will assert even though the iteration is "done",
// because the rule is that we cannot modify the hashtable while
// any iterator is still "in scope"
}
void ST_mmgc_bugzilla_637993::test4() {
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        it.nextKey();
        it.nextKey();
        add_second_half(); // this is where we break the rule
    }

    m_table.clear();
    delete elems;
    // we should never get here, the assertion should happen up above.
// line 108 "ST_mmgc_637993.st"
verifyPass(false, "false", __FILE__, __LINE__);
          ;

}
void create_mmgc_bugzilla_637993(AvmCore* core) { new ST_mmgc_bugzilla_637993(core); }
}
}
#endif

// Generated from ST_mmgc_basics.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_basics {
class ST_mmgc_basics : public Selftest {
public:
ST_mmgc_basics(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();
void test9();
void test10();
void test11();
void test12();
void test13();
void test14();
void test15();
void test16();
void test17();
private:
    MMgc::GC *gc;
    MMgc::FixedAlloc *fa;
    MMgc::FixedMalloc *fm;

};
ST_mmgc_basics::ST_mmgc_basics(AvmCore* core)
    : Selftest(core, "mmgc", "basics", ST_mmgc_basics::ST_names,ST_mmgc_basics::ST_explicits)
{}
const char* ST_mmgc_basics::ST_names[] = {"create_gc_instance","create_gc_object","get_bytesinuse","collect","getgcheap","fixedAlloc","fixedMalloc","gcheap","gcheapAlign","gcmethods","finalizerAlloc","finalizerDelete","nestedGCs","collectDormantGC","lockObject","regression_551169","blacklisting","get_bytesinusefast", NULL };
const bool ST_mmgc_basics::ST_explicits[] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false, false };
void ST_mmgc_basics::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
case 5: test5(); return;
case 6: test6(); return;
case 7: test7(); return;
case 8: test8(); return;
case 9: test9(); return;
case 10: test10(); return;
case 11: test11(); return;
case 12: test12(); return;
case 13: test13(); return;
case 14: test14(); return;
case 15: test15(); return;
case 16: test16(); return;
case 17: test17(); return;
}
}
void ST_mmgc_basics::prologue() {
    MMgc::GCConfig config;
    gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    if (gc==NULL) {
        MMgc::GCHeap::Init();
        gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    }

}
void ST_mmgc_basics::epilogue() {
delete gc;

}
using namespace MMgc;
class DeleteInFinalizer : public GCFinalizedObject {
 public:
  // note "small" is a reserved identifier in Visual C++ for Windows Mobile (#defined to be 'char')
  DeleteInFinalizer(GCFinalizedObject *big, GCFinalizedObject *small_) : big(big), small_(small_) {};
  ~DeleteInFinalizer() { delete big; delete small_; }
 private:
  GCFinalizedObject *big;
  GCFinalizedObject *small_;
};

// Any small object would do
class AllocInFinalizer2 : public GCObject {
public:
    void* dummy;
};

class AllocInFinalizer : public GCFinalizedObject {
public:
    AllocInFinalizer() {}
    ~AllocInFinalizer() { new (GC::GetGC(this)) AllocInFinalizer2(); }
};

class LockableObject : public GCFinalizedObject {
public:
    LockableObject(int* counter) : counter(counter) {}
    virtual ~LockableObject() {
        *counter = *counter + 1;
    }
    int * const counter;
};

class LockerAndUnlocker
{
public:   
    static const int numlocked = 100;
    static GCObjectLock* lock[numlocked];
    static GCObjectLock* lock2[numlocked];
    static int counter;
    
    static bool createAndLockObjects(GC* gc) {
        counter = 0;
        for ( int i=0 ; i < numlocked ; i++ )
            lock[i] = gc->LockObject(new (gc) LockableObject(&counter));
        return true;
    }

    static bool lockLevel2(GC* gc) {
        for ( int i=0 ; i < numlocked ; i++ )
            lock2[i] = gc->LockObject(gc->GetLockedObject(lock[i]));
        return true;
    }

    static bool testLocksHeld(GC* gc, int level) {
        if (counter != 0)
            return false;

        int held = 0;
        for ( int i=0 ; i < numlocked ; i++ )
            held += bool(gc->GetLockedObject(lock[i]) != NULL);
        if (level > 1)
            for ( int i=0 ; i < numlocked ; i++ )
                held += bool(gc->GetLockedObject(lock2[i]) != NULL);

        if (held != level*numlocked)
            return false;
            
        return true;
    }
    
    static bool testLocksNotHeld(GC*) {
        // At least some of the destructors should have run...
        if (counter < numlocked/2)
            return false;
        return true;
    }

    static bool unlockLevel2(GC* gc) {
        for ( int i=0 ; i < numlocked ; i++ )
            gc->UnlockObject(lock2[i]);
        return true;
    }

    static bool unlockLevel1(GC* gc) {
        for ( int i=0 ; i < numlocked ; i++ ) {
#ifdef MMGC_HEAP_GRAPH
            gc->addToBlacklist(gc->GetLockedObject(lock[i]));
#endif
            gc->UnlockObject(lock[i]);
        }
        return true;
    }
};

GCObjectLock* LockerAndUnlocker::lock[numlocked];
GCObjectLock* LockerAndUnlocker::lock2[numlocked];
int LockerAndUnlocker::counter = 0;

void ST_mmgc_basics::test0() {
// line 128 "ST_mmgc_basics.st"
verifyPass(gc != NULL, "gc != NULL", __FILE__, __LINE__);

}
void ST_mmgc_basics::test1() {
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
// line 134 "ST_mmgc_basics.st"
verifyPass(mygcobject!=NULL, "mygcobject!=NULL", __FILE__, __LINE__);
    mygcobject->i=10;
// line 136 "ST_mmgc_basics.st"
verifyPass(mygcobject->i==10, "mygcobject->i==10", __FILE__, __LINE__);

}
void ST_mmgc_basics::test2() {
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    int inuse=(int)gc->GetBytesInUse();
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
//    AvmLog("bytes in use before %d after %d\n",inuse,(int)gc->GetBytesInUse());
// line 144 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse()==inuse + sizeof(MyGCObject) + DebugSize(), "gc->GetBytesInUse()==inuse + sizeof(MyGCObject) + DebugSize()", __FILE__, __LINE__);
    delete mygcobject;

}
void ST_mmgc_basics::test3() {
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    int inuse=(int)gc->GetBytesInUse();
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
// line 152 "ST_mmgc_basics.st"
verifyPass((int)gc->GetBytesInUse()>inuse, "(int)gc->GetBytesInUse()>inuse", __FILE__, __LINE__);
    delete mygcobject;
//    AvmLog("collect: inuse=%d current=%d\n",inuse,(int)gc->GetBytesInUse());
    gc->Collect();
//    AvmLog("collect: inuse=%d current=%d\n",inuse,(int)gc->GetBytesInUse());
// line 157 "ST_mmgc_basics.st"
verifyPass((int)gc->GetBytesInUse()<=inuse, "(int)gc->GetBytesInUse()<=inuse", __FILE__, __LINE__);

}
void ST_mmgc_basics::test4() {
// line 160 "ST_mmgc_basics.st"
verifyPass(gc->GetGCHeap()!=NULL, "gc->GetGCHeap()!=NULL", __FILE__, __LINE__);

}
void ST_mmgc_basics::test5() {
    MMgc::FixedAlloc *fa;
    fa=new MMgc::FixedAlloc(2048,MMgc::GCHeap::GetGCHeap());
// line 165 "ST_mmgc_basics.st"
verifyPass((int)fa->GetMaxAlloc()==0, "(int)fa->GetMaxAlloc()==0", __FILE__, __LINE__);
// line 166 "ST_mmgc_basics.st"
verifyPass((int)fa->GetNumBlocks()==0, "(int)fa->GetNumBlocks()==0", __FILE__, __LINE__);
    void *data1=fa->Alloc(2048);
// line 168 "ST_mmgc_basics.st"
verifyPass(MMgc::FixedAlloc::GetFixedAlloc(data1)==fa, "MMgc::FixedAlloc::GetFixedAlloc(data1)==fa", __FILE__, __LINE__);
// line 169 "ST_mmgc_basics.st"
verifyPass(fa->GetBytesInUse()==DebugSize()+2048, "fa->GetBytesInUse()==DebugSize()+2048", __FILE__, __LINE__);
// line 170 "ST_mmgc_basics.st"
verifyPass(fa->GetItemSize()==2048, "fa->GetItemSize()==2048", __FILE__, __LINE__);
    void *data2=fa->Alloc(2048);
// line 172 "ST_mmgc_basics.st"
verifyPass(MMgc::FixedAlloc::GetFixedAlloc(data2)==fa, "MMgc::FixedAlloc::GetFixedAlloc(data2)==fa", __FILE__, __LINE__);
//    AvmLog("fa->GetItemSize=%d\n",(int)fa->GetItemSize());
// line 174 "ST_mmgc_basics.st"
verifyPass((int)fa->GetItemSize()==2048, "(int)fa->GetItemSize()==2048", __FILE__, __LINE__);
    fa->Free(data1);
// line 176 "ST_mmgc_basics.st"
verifyPass((int)fa->GetItemSize()==2048, "(int)fa->GetItemSize()==2048", __FILE__, __LINE__);
// line 177 "ST_mmgc_basics.st"
verifyPass((int)fa->GetMaxAlloc()==1, "(int)fa->GetMaxAlloc()==1", __FILE__, __LINE__);
// line 178 "ST_mmgc_basics.st"
verifyPass((int)fa->GetNumBlocks()==1, "(int)fa->GetNumBlocks()==1", __FILE__, __LINE__);
    fa->Free(data2);
    delete fa;

}
void ST_mmgc_basics::test6() {
    fm=MMgc::FixedMalloc::GetFixedMalloc();
    int start=(int)fm->GetBytesInUse();
    int starttotal=(int)fm->GetTotalSize();
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
// line 187 "ST_mmgc_basics.st"
verifyPass((int)fm->GetBytesInUse()==start, "(int)fm->GetBytesInUse()==start", __FILE__, __LINE__);
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
// line 189 "ST_mmgc_basics.st"
verifyPass((int)fm->GetTotalSize()==starttotal, "(int)fm->GetTotalSize()==starttotal", __FILE__, __LINE__);
    void *obj=fm->Alloc(8192);
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
//    %%verify fm->GetBytesInUse()==start + 8192 + MMgc::DebugSize()
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
//    %%verify (int)fm->GetTotalSize()==starttotal+2
    fm->Free(obj);
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
// line 197 "ST_mmgc_basics.st"
verifyPass((int)fm->GetBytesInUse()==start, "(int)fm->GetBytesInUse()==start", __FILE__, __LINE__);
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
// line 199 "ST_mmgc_basics.st"
verifyPass((int)fm->GetTotalSize()==starttotal, "(int)fm->GetTotalSize()==starttotal", __FILE__, __LINE__);
    obj=fm->Calloc(1024,10);
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
// FixedMalloc is currently (as of redux 3229) tracking large allocs using a list of
// small objects, in some debug modes.  So we can't have a precise test here.
// line 204 "ST_mmgc_basics.st"
verifyPass((int)fm->GetBytesInUse()>=start+1024*12 && (int)fm->GetBytesInUse()<=start+1024*12+64, "(int)fm->GetBytesInUse()>=start+1024*12 && (int)fm->GetBytesInUse()<=start+1024*12+64", __FILE__, __LINE__);
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
// line 206 "ST_mmgc_basics.st"
verifyPass((int)fm->GetTotalSize()==starttotal+3, "(int)fm->GetTotalSize()==starttotal+3", __FILE__, __LINE__);
    fm->Free(obj);
// line 208 "ST_mmgc_basics.st"
verifyPass((int)fm->GetBytesInUse()==start, "(int)fm->GetBytesInUse()==start", __FILE__, __LINE__);
// line 209 "ST_mmgc_basics.st"
verifyPass((int)fm->GetTotalSize()==starttotal, "(int)fm->GetTotalSize()==starttotal", __FILE__, __LINE__);

}
void ST_mmgc_basics::test7() {
    MMgc::GCHeap *gh=MMgc::GCHeap::GetGCHeap();
    int startfreeheap=(int)gh->GetFreeHeapSize();
//    %%verify (int)gh->GetTotalHeapSize()==128
//    AvmLog("gh->GetFreeHeapSize()=%d\n",(int)gh->GetFreeHeapSize());
// line 216 "ST_mmgc_basics.st"
verifyPass((int)gh->GetFreeHeapSize()==startfreeheap, "(int)gh->GetFreeHeapSize()==startfreeheap", __FILE__, __LINE__);
//gh->Config().heapLimit = 1024;
//    %%verify (int)gh->GetTotalHeapSize()==128
//    AvmLog("gh->GetFreeHeapSize()=%d\n",(int)gh->GetFreeHeapSize());
// line 220 "ST_mmgc_basics.st"
verifyPass((int)gh->GetFreeHeapSize()==startfreeheap, "(int)gh->GetFreeHeapSize()==startfreeheap", __FILE__, __LINE__);
    void *data = gh->Alloc(10,MMgc::GCHeap::kExpand | MMgc::GCHeap::kZero);
// line 222 "ST_mmgc_basics.st"
verifyPass((int)gh->GetTotalHeapSize()>startfreeheap, "(int)gh->GetTotalHeapSize()>startfreeheap", __FILE__, __LINE__);
//    AvmLog("gh->GetFreeHeapSize()=%d\n",(int)gh->GetFreeHeapSize());
    gh->FreeNoProfile(data);
       
}
void ST_mmgc_basics::test8() {
    MMgc::GCHeap *gh=MMgc::GCHeap::GetGCHeap();

    // Tricky: try to provoke some internal asserts
    void *d[1000];
    for ( unsigned i=0 ; i < ARRAY_SIZE(d) ; i++ ) {
        d[i] = gh->Alloc(1);
        void *data = gh->Alloc(10,MMgc::GCHeap::flags_Alloc, 4);
        gh->Free(data);
    }
    for ( unsigned i=0 ; i < ARRAY_SIZE(d) ; i++ )
        gh->Free(d[i]);

    // 
    for ( size_t k=2 ; k <= 256 ; k *= 2 ) {
        void *data = gh->Alloc(10,MMgc::GCHeap::flags_Alloc, k);
// line 242 "ST_mmgc_basics.st"
verifyPass(((uintptr_t)data & (k*MMgc::GCHeap::kBlockSize - 1)) == 0, "((uintptr_t)data & (k*MMgc::GCHeap::kBlockSize - 1)) == 0", __FILE__, __LINE__);
// line 243 "ST_mmgc_basics.st"
verifyPass(gh->Size(data) == 10, "gh->Size(data) == 10", __FILE__, __LINE__);
        gh->Free(data);
    }

}
void ST_mmgc_basics::test9() {
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
// line 251 "ST_mmgc_basics.st"
verifyPass((MyGCObject *)gc->FindBeginningGuarded(mygcobject)==mygcobject, "(MyGCObject *)gc->FindBeginningGuarded(mygcobject)==mygcobject", __FILE__, __LINE__);
// line 252 "ST_mmgc_basics.st"
verifyPass((MyGCObject *)gc->FindBeginningFast(mygcobject)==mygcobject, "(MyGCObject *)gc->FindBeginningFast(mygcobject)==mygcobject", __FILE__, __LINE__);

// Bugzilla 542529 - in debug mode we would assert here due to logic flaws in the allocatr
}
void ST_mmgc_basics::test10() {
    MMGC_GCENTER(gc);
    new (gc) AllocInFinalizer();
    gc->Collect(false);
// line 259 "ST_mmgc_basics.st"
verifyPass(true, "true", __FILE__, __LINE__);

}
void ST_mmgc_basics::test11() {
    MMGC_GCENTER(gc);
    new (gc) DeleteInFinalizer(new (gc, 100) GCFinalizedObject(), new (gc) GCFinalizedObject());
    //delete m; delete m; // this verifies we crash, it does
    gc->Collect(false);
// line 266 "ST_mmgc_basics.st"
verifyPass(true, "true", __FILE__, __LINE__);
    GCFinalizedObject *gcfo = new (gc) GCFinalizedObject();
    gcfo->~GCFinalizedObject();
    gcfo->~GCFinalizedObject(); // this used to be a deleteing dtor and would crash, not anymore



}
void ST_mmgc_basics::test12() {
    GCConfig config;
    GC *gcb = new GC(GCHeap::GetGCHeap(), config);
    MMGC_GCENTER(gc);
    void *a = gc->Alloc(8);
    {
        MMGC_GCENTER(gcb);
        a = gcb->Alloc(8);
        {
            MMGC_GCENTER(gc);
            a = gc->Alloc(8);
        }
        a = gcb->Alloc(8);
    }
    a = gc->Alloc(8);
    // just fishing for asserts/hangs/crashes
// line 289 "ST_mmgc_basics.st"
verifyPass(true, "true", __FILE__, __LINE__);
    delete gcb;

}
void ST_mmgc_basics::test13() {
    {
        GCConfig config;
        GC *gcb = new GC(GCHeap::GetGCHeap(), config);
        {
            MMGC_GCENTER(gcb);
            gcb->Alloc(8);
        }

        // this will cause a Collection in gcb
        GCHeap::SignalExternalFreeMemory();
        delete gcb;

        // just fishing for asserts/hangs/crashes
// line 306 "ST_mmgc_basics.st"
verifyPass(true, "true", __FILE__, __LINE__);
    }

}
void ST_mmgc_basics::test14() {
    {
        GCConfig config;
        GC *gc = new GC(GCHeap::GetGCHeap(), config);
        MMGC_GCENTER(gc);

// line 315 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::createAndLockObjects(gc), "LockerAndUnlocker::createAndLockObjects(gc)", __FILE__, __LINE__);
        gc->Collect();
        gc->Collect();
// line 318 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::testLocksHeld(gc, 1), "LockerAndUnlocker::testLocksHeld(gc, 1)", __FILE__, __LINE__);
        gc->Collect();
        gc->Collect();
// line 321 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::lockLevel2(gc), "LockerAndUnlocker::lockLevel2(gc)", __FILE__, __LINE__);
        gc->Collect();
        gc->Collect();
// line 324 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::testLocksHeld(gc, 2), "LockerAndUnlocker::testLocksHeld(gc, 2)", __FILE__, __LINE__);
        gc->Collect();
        gc->Collect();
// line 327 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::unlockLevel2(gc), "LockerAndUnlocker::unlockLevel2(gc)", __FILE__, __LINE__);
        gc->Collect();
        gc->Collect();
// line 330 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::testLocksHeld(gc, 1), "LockerAndUnlocker::testLocksHeld(gc, 1)", __FILE__, __LINE__);
        gc->Collect();
        gc->Collect();
// line 333 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::unlockLevel1(gc), "LockerAndUnlocker::unlockLevel1(gc)", __FILE__, __LINE__);

        // Bug 637695: be aware: conservative retention may foil verify below
        gc->Collect();
        gc->Collect();
// line 338 "ST_mmgc_basics.st"
verifyPass(LockerAndUnlocker::testLocksNotHeld(gc), "LockerAndUnlocker::testLocksNotHeld(gc)", __FILE__, __LINE__);

        delete gc;
    }

}
void ST_mmgc_basics::test15() {
    {
        GCConfig config;
        GC *testGC = new GC(GCHeap::GetGCHeap(), config);
        {
            MMGC_GCENTER(testGC);
            testGC->StartIncrementalMark();
            // self test for tricky GCRoot deletion logic
            // do this a bunch, idea is to try to hit GetItemAbove border edge cases
            //GCMarkStack& ms = testGC->m_incrementalWork;
            for(int i=0;i<10000;i++) {
                //GCRoot *fauxRoot = new GCRoot(testGC, new char[GC::kMarkItemSplitThreshold*2], GC::kMarkItemSplitThreshold*2);
                //testGC->MarkAllRoots();
                // tail of fauxRoot is on stack
                //uintptr_t sentinel = fauxRoot->GetMarkStackSentinelPointer();
                //if(sentinel) {
                //    const void* ptr;
                //    ms.Read_RootProtector(sentinel, ptr);
                //    %%verify ptr == fauxRoot
                //    uintptr_t tail = ms.GetItemAbove(sentinel);
                //    %%verify ms.GetEndAt(tail) == fauxRoot->End()
                //    %%verify sentinel != 0
                //}
                //delete [] (char*)fauxRoot->Get();
                //delete fauxRoot;
                //if(sentinel) {
                //    %%verify ms.P(sentinel) == GCMarkStack::kDeadItem
                //    %%verify ms.GetSentinel1TypeAt(ms.GetItemAbove(sentinel)) == GCMarkStack::kDeadItem
                //}
// line 372 "ST_mmgc_basics.st"
verifyPass(true, "true", __FILE__, __LINE__);
            }
            testGC->Mark();
            testGC->ClearMarkStack();
            testGC->ClearMarks();
        }
        delete testGC;
    }


}
void ST_mmgc_basics::test16() {
#ifdef MMGC_HEAP_GRAPH
    GCConfig config;
    GC *gc = new GC(GCHeap::GetGCHeap(), config);
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
    gc->addToBlacklist(mygcobject);
    gc->Collect();
    gc->removeFromBlacklist(mygcobject);
    delete gc;
#endif
// line 394 "ST_mmgc_basics.st"
verifyPass(true, "true", __FILE__, __LINE__);

}
void ST_mmgc_basics::test17() {
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *) new (gc) MyGCObject();
// line 400 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse() == gc->GetBytesInUseFast(), "gc->GetBytesInUse() == gc->GetBytesInUseFast()", __FILE__, __LINE__);
    delete mygcobject;
// line 402 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse() == gc->GetBytesInUseFast(), "gc->GetBytesInUse() == gc->GetBytesInUseFast()", __FILE__, __LINE__);
    gc->Collect();
// line 404 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse() == gc->GetBytesInUseFast(), "gc->GetBytesInUse() == gc->GetBytesInUseFast()", __FILE__, __LINE__);
    MyGCLargeObject *mygclargeobject;
    mygclargeobject = (MyGCLargeObject *) new (gc) MyGCLargeObject();
// line 407 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse() == gc->GetBytesInUseFast(), "gc->GetBytesInUse() == gc->GetBytesInUseFast()", __FILE__, __LINE__);
    delete mygclargeobject;
// line 409 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse() == gc->GetBytesInUseFast(), "gc->GetBytesInUse() == gc->GetBytesInUseFast()", __FILE__, __LINE__);
    gc->Collect();
// line 411 "ST_mmgc_basics.st"
verifyPass(gc->GetBytesInUse() == gc->GetBytesInUseFast() , "gc->GetBytesInUse() == gc->GetBytesInUseFast() ", __FILE__, __LINE__);
     

}
void create_mmgc_basics(AvmCore* core) { new ST_mmgc_basics(core); }
}
}
#endif

// Generated from ST_mmgc_dependent.st
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

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_dependent {
using namespace MMgc;

static const size_t nbytes = 16*1024*1024;

class DependentAllocHolder : public GCFinalizedObject {
public:

  DependentAllocHolder() {
    memory = (char*)FixedMalloc::GetFixedMalloc()->Alloc(nbytes);
    GC::GetGC(this)->SignalDependentAllocation(nbytes);
  }

  virtual ~DependentAllocHolder() {
    FixedMalloc::GetFixedMalloc()->Free(memory);
    memory = NULL;
    GC::GetGC(this)->SignalDependentDeallocation(nbytes);
  }
  private:
    char* memory;
};

class ST_mmgc_dependent : public Selftest {
public:
ST_mmgc_dependent(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
private:
    MMgc::GC *gc;

};
ST_mmgc_dependent::ST_mmgc_dependent(AvmCore* core)
    : Selftest(core, "mmgc", "dependent", ST_mmgc_dependent::ST_names,ST_mmgc_dependent::ST_explicits)
{}
const char* ST_mmgc_dependent::ST_names[] = {"dependent_alloc","dependent_memory_total","dependent_memory_unknown", NULL };
const bool ST_mmgc_dependent::ST_explicits[] = {false,false,false, false };
void ST_mmgc_dependent::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
}
}
void ST_mmgc_dependent::prologue() {
    GCConfig config;
    gc = new GC(GCHeap::GetGCHeap(), config);

}
void ST_mmgc_dependent::epilogue() {
    delete gc;

}
void ST_mmgc_dependent::test0() {
    size_t maxheap = 0;
    {
        MMGC_GCENTER(gc);

        int count = 100;
        for (int c=0; c<count; c++) {
            (void)(new (gc) DependentAllocHolder());
            size_t heapsize = gc->policy.blocksOwnedByGC() * GCHeap::kBlockSize;
            // printf("%lu\n", (unsigned long)heapsize);
            if (heapsize > maxheap)
                maxheap = heapsize;
        }
    }

    // This is tricky to get right but for this test the 16MB blocks will dominate
    // completely.  So assume that heap size must stay below L*2*16MB for the
    // L that applies at 32MB.

// line 68 "ST_mmgc_dependent.st"
verifyPass(size_t(gc->policy.queryLoadForHeapsize(double(2*nbytes)) * 2.0 * double(nbytes)) >= maxheap, "size_t(gc->policy.queryLoadForHeapsize(double(2*nbytes)) * 2.0 * double(nbytes)) >= maxheap", __FILE__, __LINE__);

}
void ST_mmgc_dependent::test1() {
#if defined VMCFG_TELEMETRY && defined AVMSHELL_BUILD
    size_t depMem_start,depMem_end;
    depMem_start = depMem_end = 0;
    MMgc::GC* gc = core->gc;

    // Check total dependent memory consistency
    for(int i = 0; i < MMgc::typeCount; i++)
        depMem_start += gc->getDependentMemory((MMgc::DependentMemoryType)i);
// line 79 "ST_mmgc_dependent.st"
verifyPass(depMem_start == gc->policy.dependentAllocation, "depMem_start == gc->policy.dependentAllocation", __FILE__, __LINE__);

    // Check byteArray type dependent memory
    avmshell::ShellCore* c = (avmshell::ShellCore*)core;
    avmshell::ShellToplevel* top = c->shell_toplevel;

    size_t byteArray_bytes1 = gc->getDependentMemory(MMgc::typeByteArray);
    ByteArrayObject* byteArray = top->byteArrayClass()->constructByteArray();
    byteArray->writeBoolean(false);
    byteArray->writeDouble(3.14);

    // Allocate known type dependent memory
    size_t byteArray_bytes2 = gc->getDependentMemory(MMgc::typeByteArray);
// line 92 "ST_mmgc_dependent.st"
verifyPass(byteArray_bytes2 > byteArray_bytes1, "byteArray_bytes2 > byteArray_bytes1", __FILE__, __LINE__);

    // Allocate unknown dependent memory
    DependentAllocHolder* obj = new (gc) DependentAllocHolder();

    size_t byteArray_bytes3 = gc->getDependentMemory(MMgc::typeByteArray);
// line 98 "ST_mmgc_dependent.st"
verifyPass(byteArray_bytes3 == byteArray_bytes2, "byteArray_bytes3 == byteArray_bytes2", __FILE__, __LINE__);
    byteArray->clear();
 
// line 101 "ST_mmgc_dependent.st"
verifyPass(gc->getDependentMemory(MMgc::typeByteArray) == byteArray_bytes1, "gc->getDependentMemory(MMgc::typeByteArray) == byteArray_bytes1", __FILE__, __LINE__);
    delete obj;

    // Consistency check
    for(int i = 0; i < MMgc::typeCount; i++)
        depMem_end += gc->getDependentMemory((MMgc::DependentMemoryType)i);
// line 107 "ST_mmgc_dependent.st"
verifyPass(depMem_end == gc->policy.dependentAllocation, "depMem_end == gc->policy.dependentAllocation", __FILE__, __LINE__);
// line 108 "ST_mmgc_dependent.st"
verifyPass(depMem_end == depMem_start, "depMem_end == depMem_start", __FILE__, __LINE__);

#if !defined DEBUG && !defined DEBUGGER
    // Get memory for out of bounds type
    // getDependentMemory has assert; skip in debug build
// line 113 "ST_mmgc_dependent.st"
verifyPass(gc->getDependentMemory(MMgc::typeCount) == 0, "gc->getDependentMemory(MMgc::typeCount) == 0", __FILE__, __LINE__);
#endif
// line 115 "ST_mmgc_dependent.st"
verifyPass(true, "true", __FILE__, __LINE__);
#endif

}
void ST_mmgc_dependent::test2() {
#if defined VMCFG_TELEMETRY && defined AVMSHELL_BUILD
    // Check unknown type dependent memory
    MMgc::GC* gc = core->gc;
    size_t unknownDependentMem_start = gc->getDependentMemory(MMgc::typeUnknown);
    DependentAllocHolder* obj = new (gc) DependentAllocHolder();
    size_t unknownDependentMem_end = gc->getDependentMemory(MMgc::typeUnknown);
// line 125 "ST_mmgc_dependent.st"
verifyPass((unknownDependentMem_end - unknownDependentMem_start) == nbytes, "(unknownDependentMem_end - unknownDependentMem_start) == nbytes", __FILE__, __LINE__);
    
    delete obj;
// line 128 "ST_mmgc_dependent.st"
verifyPass(unknownDependentMem_start == gc->getDependentMemory(MMgc::typeUnknown), "unknownDependentMem_start == gc->getDependentMemory(MMgc::typeUnknown)", __FILE__, __LINE__);
#else
// line 130 "ST_mmgc_dependent.st"
verifyPass(true, "true", __FILE__, __LINE__);
#endif


}
void create_mmgc_dependent(AvmCore* core) { new ST_mmgc_dependent(core); }
}
}
#endif

// Generated from ST_mmgc_exact.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_exact {

class Cthulhu : public MMgc::GCTraceableObject
{
public:
    static Cthulhu* create(MMgc::GC* gc) {
        return new (gc, MMgc::kExact) Cthulhu();
    }

    virtual bool gcTrace(MMgc::GC* gc, size_t cursor) {
        (void)gc;
        // Incorrectly say that there's more to do even when the object is small
        if (cursor == 0)
            return true;
        // Concede
        return false;
    }
};

// This test triggers a particular edge case in GC::SplitExactGCObject where
// a small object that returns 'true' from its gcTrace method is synchronously
// marked until the method returns 'false'; a 'true' return is only ever expected
// from the gcTrace methods of large objects.

class ST_mmgc_exact : public Selftest {
public:
ST_mmgc_exact(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
};
ST_mmgc_exact::ST_mmgc_exact(AvmCore* core)
    : Selftest(core, "mmgc", "exact", ST_mmgc_exact::ST_names,ST_mmgc_exact::ST_explicits)
{}
const char* ST_mmgc_exact::ST_names[] = {"IncorrectlySplitSmallObject", NULL };
const bool ST_mmgc_exact::ST_explicits[] = {false, false };
void ST_mmgc_exact::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_mmgc_exact::test0() {
    // Create an object that stays alive so that its tracer will be called
    MMgc::GCObjectLock* lock = core->gc->LockObject(Cthulhu::create(core->gc));
    core->gc->Collect();
    core->gc->Collect();
    core->gc->UnlockObject(lock);

// line 42 "ST_mmgc_exact.st"
verifyPass(true, "true", __FILE__, __LINE__);


}
void create_mmgc_exact(AvmCore* core) { new ST_mmgc_exact(core); }
}
}
#endif

// Generated from ST_mmgc_externalalloc.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_externalalloc {
using namespace MMgc;
class MyCallback : public MMgc::OOMCallback
{
public:
    MMgc::MemoryStatus status;

    MyCallback()
    {
        status = MMgc::GCHeap::GetGCHeap()->GetStatus();
        MMgc::GCHeap::GetGCHeap()->AddOOMCallback(this);
    }

    void memoryStatusChange(MMgc::MemoryStatus /*oldStatus*/, MMgc::MemoryStatus newStatus)
    {
        status = newStatus;
    }
};

class ST_mmgc_externalalloc : public Selftest {
public:
ST_mmgc_externalalloc(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
private:
    MyCallback *cb;

};
ST_mmgc_externalalloc::ST_mmgc_externalalloc(AvmCore* core)
    : Selftest(core, "mmgc", "externalalloc", ST_mmgc_externalalloc::ST_names,ST_mmgc_externalalloc::ST_explicits)
{}
const char* ST_mmgc_externalalloc::ST_names[] = {"externalAllocation", NULL };
const bool ST_mmgc_externalalloc::ST_explicits[] = {false, false };
void ST_mmgc_externalalloc::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_mmgc_externalalloc::prologue() {
    cb = new MyCallback();

}
void ST_mmgc_externalalloc::epilogue() {
    cb = NULL;

}
void ST_mmgc_externalalloc::test0() {
    MMGC_GCENTER(core->gc);
    size_t softlimit = GCHeap::GetGCHeap()->Config().heapSoftLimit;
    // Remove the heapSoftLimit, this should ensure that we are not in a MMgc::kMemSoftLimit state,
    // and hopefully we are in a kMemNormal state otherwise we are in a kMemAbort state and all bets are off.
    GCHeap::GetGCHeap()->Config().heapSoftLimit = 0;
// line 45 "ST_mmgc_externalalloc.st"
verifyPass(cb->status==MMgc::kMemNormal, "cb->status==MMgc::kMemNormal", __FILE__, __LINE__);
    GCHeap::GetGCHeap()->Config().heapSoftLimit = GCHeap::GetGCHeap()->GetTotalHeapSize();
    MMgc::GCHeap::SignalExternalAllocation(1024*1024);
// line 48 "ST_mmgc_externalalloc.st"
verifyPass(cb->status==MMgc::kMemSoftLimit, "cb->status==MMgc::kMemSoftLimit", __FILE__, __LINE__);
    MMgc::GCHeap::SignalExternalDeallocation(1024*1024);
// line 50 "ST_mmgc_externalalloc.st"
verifyPass(cb->status==MMgc::kMemNormal, "cb->status==MMgc::kMemNormal", __FILE__, __LINE__);
    GCHeap::GetGCHeap()->Config().heapSoftLimit = softlimit;
    softlimit = 0;



}
void create_mmgc_externalalloc(AvmCore* core) { new ST_mmgc_externalalloc(core); }
}
}
#endif

// Generated from ST_mmgc_finalize_uninit.st
// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */

// Bugzilla 573737 - a throw from an argument to a constructor can
// cause an object to be allocated on the gc-heap before it has been
// fully initialized.  (In particular, its vtable could be missing.)
// Since finalizers use the virtual destructor method, a missing
// vtable is a problem.
//
// Its a little tricky to test this because the "order of evaluation
// to an operator new() to get memory and the evaluation of arguments
// to constructors is undefined"; see details below.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_finalize_uninit {

using namespace MMgc;

class D : public GCFinalizedObject
{
public:
    D(int dummyArgument) { (void)dummyArgument; }
    ~D() { ++finalized; }
    static int finalized_count() { return D::finalized; }
    static void reset_finalized_count() { finalized = 0; }
    // "usual" overload of placement new used in mmgc client code
    void * operator new(size_t size, GC *gc) {
        return GCFinalizedObject::operator new(size, gc);
    }
    // hack to explicitly order new-allocation + ctor-arg eval; see below
    // (but reallys its just good ol' placement new!)
    void * operator new(size_t size, GC *gc, void *raw) {
        (void)size; (void)gc;
        return raw;
    }
private:
    static int finalized;
};

/*static*/
int D::finalized;

int deathValue(AvmCore *core)
{
    core->throwAtom(atomFromIntptrValue(1)); /* won't return */

    return 0; /* silence compiler warnings */
}

// Test checks that finalizers themselves have not broken; here,
// constructor argument subexpression evaluates cleanly.
class ST_mmgc_finalize_uninit : public Selftest {
public:
ST_mmgc_finalize_uninit(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
};
ST_mmgc_finalize_uninit::ST_mmgc_finalize_uninit(AvmCore* core)
    : Selftest(core, "mmgc", "finalize_uninit", ST_mmgc_finalize_uninit::ST_names,ST_mmgc_finalize_uninit::ST_explicits)
{}
const char* ST_mmgc_finalize_uninit::ST_names[] = {"check_finalizers_still_work","original_death","desugared_check_finalizers_still_work","desugared_death", NULL };
const bool ST_mmgc_finalize_uninit::ST_explicits[] = {false,false,false,false, false };
void ST_mmgc_finalize_uninit::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
}
}
void ST_mmgc_finalize_uninit::test0() {
{
    D::reset_finalized_count();
    D* d;
    int i;

    // loop to alloc many (and subsequently reclaim >=expected percentage)
    for (i = 0; i < 100; i++) {
        d = new (core->gc) D(1);
    }
    (void) d;

    core->gc->Collect(); // finish any prior incremental work ...
    core->gc->Collect(); // ... and ensure we got fresh + complete gc.

    // printf("D::finalized_count(): %d\n", D::finalized_count());

// line 75 "ST_mmgc_finalize_uninit.st"
verifyPass((D::finalized_count() > 90), "(D::finalized_count() > 90)", __FILE__, __LINE__);
}

// Test illustrates of the kind of code that exposed the original bug;
// here, constructor argument subexpression throws.
}
void ST_mmgc_finalize_uninit::test1() {
{
    D* d;
    volatile int i;

    // Here, constructor argument subexpression throws

    // Just one allocation attempt alone would risk false retention of
    // intermediate values, so loop a bit to ensure that *some* D's,
    // if allocated at all, will be considered garbage.
    for (i = 0; i < 5; i++) {
        TRY (core, kCatchAction_Ignore) {
            d = new (core->gc) D(deathValue(core));
        }  CATCH (Exception *e) {
            (void)e;
        }
        END_CATCH
        END_TRY
    }
    (void) d;

    // if things go badly, one of the collections below will segfault
    // during finalization.
    core->gc->Collect();
    core->gc->Collect();

    // (not dying is passing.)
// line 107 "ST_mmgc_finalize_uninit.st"
verifyPass(true, "true", __FILE__, __LINE__);
}

// C++ standard says "The order of evaluation to an operator new() to
// get memory and the evaluation of arguments to constructors is
// undefined."
//
// Unfortunately, it is difficult to directly express the particular
// order of evaluation that exposes the bug in question, because
// allocation and construction are tied together.
//
// So, here we manually decompose the tests above to control
// evaluation order of operator new() and constructor arguments, to
// express suitable evil (namely: allocation; args eval; construction)
// for selftest.
//
// The desugaring iteself is:
// desugar[[ new (gc-exp) D(arg-exp) ]]
// ==> mem = D::operator new(sizeof(D), arg), new (gc-exp, mem) D(arg-exp)

// Test illustrates desugaring is "sound"; keep in sync with
// check_finalizers_still_work above.
}
void ST_mmgc_finalize_uninit::test2() {
{
    D::reset_finalized_count();
    D* d;
    int i;

    // loop to alloc many (and subsequently reclaim >=expected percentage)
    for (i = 0; i < 100; i++) {
        // d = new (core->gc) D(s);
        void *mem = D::operator new(sizeof(D), core->gc);
        d = new (core->gc, mem) D(1);
    }
    (void) d;

    core->gc->Collect(); // finish any prior incremental work ...
    core->gc->Collect(); // ... and ensure we got fresh + complete gc.

    // printf("D::finalized_count(): %d\n", D::finalized_count());

// line 148 "ST_mmgc_finalize_uninit.st"
verifyPass((D::finalized_count() > 90), "(D::finalized_count() > 90)", __FILE__, __LINE__);
}


// Test forces evil order of evaluation via desugaring of
// construction; keep in sync with original_death above.
}
void ST_mmgc_finalize_uninit::test3() {
{
    D* d;
    volatile int i;

    // Here, constructor argument subexpression throws

    // Just one allocation attempt alone would risk false retention of
    // intermediate values, so loop a bit to ensure that *some* D's,
    // which are forcibly allocated here, will be considered garbage.
    for (i = 0; i < 5; i++) {
        TRY (core, kCatchAction_Ignore) {
            // d = new (core->gc) D(deathValue());
            void *mem = D::operator new(sizeof(D), core->gc);
            d = new (core->gc, mem) D(deathValue(core));
        }  CATCH (Exception *e) {
            (void)e;
        }
        END_CATCH
        END_TRY
    }
    (void) d;

    // if things go badly, one of the collections below will segfault
    // during finalization.
    core->gc->Collect();
    core->gc->Collect();

    // (not dying is passing.)
// line 183 "ST_mmgc_finalize_uninit.st"
verifyPass(true, "true", __FILE__, __LINE__);
}

}
void create_mmgc_finalize_uninit(AvmCore* core) { new ST_mmgc_finalize_uninit(core); }
}
}
#endif

// Generated from ST_mmgc_fixedmalloc_findbeginning.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 663508: Add FixedMalloc::FindBeginning

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_fixedmalloc_findbeginning {
using namespace MMgc;

class ST_mmgc_fixedmalloc_findbeginning : public Selftest {
public:
ST_mmgc_fixedmalloc_findbeginning(AvmCore* core);
virtual void run(int n);
virtual void prologue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();
void test9();
void test10();
void test11();

    MMgc::FixedMalloc *fm;

    // Allocates object of size sz and tests FixedMalloc::FindBeginning
    // at various addresses within the allocated object.  The sampled
    // addresses are meant to cover the following interesting edge
    // cases: at or near the object's start, the middle of the object,
    // and at or near the object's end.
    //
    // If all the tests successively identify the object's start, then
    // returns 0.  Otherwise returns a numeric code identifying which
    // tests failed.
    int allocateVerifyAndFree(size_t sz)
    {
        int retval = 0;
        (void)sz;

        char *obj = (char*)fm->Alloc(sz);

        retval |= checkLookups(obj, obj, sz);
        retval = retval << 2;
        if (sz > 1) retval |= checkLookups(obj+1, obj, sz);
        retval = retval << 2;
        if (sz > 3) retval |= checkLookups(obj+3, obj, sz);
        retval = retval << 2;
        retval |= checkLookups(obj+sz/2, obj, sz);
        retval = retval << 2;
        retval |= checkLookups(obj+sz-1, obj, sz);
        retval = retval << 2;

        fm->Free(obj);

        return retval;
    }

    // Returns 0 if both test of FixedMalloc::FindBeginning and
    // FixedMalloc::FindBeginningAndSize pass.
    // Returns 1 if the first test (of FindBeginning) fails.
    // Returns 2 if the second test (of FindBeginningAndSize) fails.
    int checkLookups(const void* probe, const void* realStart, size_t sz)
    {
        int retval = 0;
        const void* begin_recv;
        size_t size_recv;
        size_t roundup_actual_size;

        if (sz <= (size_t)FixedMalloc::kLargestAlloc)
            roundup_actual_size = fm->FindAllocatorForSize(sz)->GetItemSize();
        else
            roundup_actual_size =
                roundUp(sz + DebugSize(), GCHeap::kBlockSize) - DebugSize();

        if (fm->FindBeginning(probe) != realStart)
            retval |= 1;
        if (!fm->FindBeginningAndSize(probe, begin_recv, size_recv) ||
            (begin_recv != realStart) ||
            (size_recv != roundup_actual_size))
            retval |= 3;

        return retval;
    }

    size_t roundUp(size_t s, size_t inc) {
        return (((s + inc - 1) / inc) * inc);
    }

};
ST_mmgc_fixedmalloc_findbeginning::ST_mmgc_fixedmalloc_findbeginning(AvmCore* core)
    : Selftest(core, "mmgc", "fixedmalloc_findbeginning", ST_mmgc_fixedmalloc_findbeginning::ST_names,ST_mmgc_fixedmalloc_findbeginning::ST_explicits)
{}
const char* ST_mmgc_fixedmalloc_findbeginning::ST_names[] = {"findbeginnings_small0","findbeginnings_small1","findbeginnings_small2","findbeginnings_small3","findbeginnings_almost_large1","findbeginnings_almost_large2","findbeginnings_large","findbeginnings_almost_multiblock1","findbeginnings_almost_multiblock2","findbeginnings_barely_multiblock","findbeginnings_multiblock_two","findbeginnings_multiblock_ten", NULL };
const bool ST_mmgc_fixedmalloc_findbeginning::ST_explicits[] = {false,false,false,false,false,false,false,false,false,false,false,false, false };
void ST_mmgc_fixedmalloc_findbeginning::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
case 5: test5(); return;
case 6: test6(); return;
case 7: test7(); return;
case 8: test8(); return;
case 9: test9(); return;
case 10: test10(); return;
case 11: test11(); return;
}
}
void ST_mmgc_fixedmalloc_findbeginning::prologue() {
    fm = MMgc::FixedMalloc::GetFixedMalloc();

}

void ST_mmgc_fixedmalloc_findbeginning::test0() {
    {
        size_t sz = FixedMalloc::kSizeClasses[0];
// line 91 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test1() {
    {
        size_t sz = FixedMalloc::kSizeClasses[1];
// line 98 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test2() {
    {
        size_t sz = FixedMalloc::kSizeClasses[2];
// line 105 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test3() {
    {
        size_t sz = FixedMalloc::kSizeClasses[3];
// line 112 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test4() {
    {
        size_t sz = FixedMalloc::kLargestAlloc - 1;
        sz = sz - DebugSize();
// line 120 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 123 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test5() {
    {
        size_t sz = FixedMalloc::kLargestAlloc;
        sz = sz - DebugSize();
// line 131 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 134 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test6() {
    {
        size_t sz = FixedMalloc::kLargestAlloc+1;
        sz = sz - DebugSize();
// line 142 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 145 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test7() {
    {
        size_t sz = GCHeap::kBlockSize-1;
        sz = sz - DebugSize();
// line 153 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 156 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test8() {
    {
        size_t sz = GCHeap::kBlockSize;
        sz = sz - DebugSize();
// line 164 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 167 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test9() {
    {
        size_t sz = GCHeap::kBlockSize+1;
        sz = sz - DebugSize();
// line 175 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 178 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test10() {
    {
        size_t sz = GCHeap::kBlockSize*2;
        sz = sz - DebugSize();
// line 186 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 189 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void ST_mmgc_fixedmalloc_findbeginning::test11() {
    {
        size_t sz = GCHeap::kBlockSize*10;
        sz = sz - DebugSize();
// line 197 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
        sz = sz + DebugSize();
// line 200 "ST_mmgc_fixedmalloc_findbeginning.st"
verifyPass(allocateVerifyAndFree(sz) == 0, "allocateVerifyAndFree(sz) == 0", __FILE__, __LINE__);
              ;
    }

}
void create_mmgc_fixedmalloc_findbeginning(AvmCore* core) { new ST_mmgc_fixedmalloc_findbeginning(core); }
}
}
#endif

// Generated from ST_mmgc_gcheap.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_gcheap {
class ST_mmgc_gcheap : public Selftest {
public:
ST_mmgc_gcheap(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
};
ST_mmgc_gcheap::ST_mmgc_gcheap(AvmCore* core)
    : Selftest(core, "mmgc", "gcheap", ST_mmgc_gcheap::ST_names,ST_mmgc_gcheap::ST_explicits)
{}
const char* ST_mmgc_gcheap::ST_names[] = {"largeAlloc","largeAllocAlignment", NULL };
const bool ST_mmgc_gcheap::ST_explicits[] = {false,false, false };
void ST_mmgc_gcheap::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
}
}
using namespace MMgc;

void ST_mmgc_gcheap::test0() {
       GCHeap *heap = GCHeap::GetGCHeap();
       for(int i=1;i<4;i++) {
          void *item = heap->Alloc(GCHeap::kOSAllocThreshold*i);
// line 17 "ST_mmgc_gcheap.st"
verifyPass(heap->Size(item) == GCHeap::kOSAllocThreshold*i, "heap->Size(item) == GCHeap::kOSAllocThreshold*i", __FILE__, __LINE__);
          heap->Free(item);
       }
// line 20 "ST_mmgc_gcheap.st"
verifyPass(true, "true", __FILE__, __LINE__);

}
void ST_mmgc_gcheap::test1() {
       GCHeap *heap = GCHeap::GetGCHeap();
       for(int i=1;i<10;i++) {
          void *item = heap->Alloc(GCHeap::kOSAllocThreshold*i, GCHeap::flags_Alloc, 1<<i);
// line 26 "ST_mmgc_gcheap.st"
verifyPass(heap->Size(item) == GCHeap::kOSAllocThreshold*i, "heap->Size(item) == GCHeap::kOSAllocThreshold*i", __FILE__, __LINE__);
// line 27 "ST_mmgc_gcheap.st"
verifyPass(uintptr_t(((GCHeap::kBlockSize<<(i-1))-1) & uintptr_t(item)) == 0, "uintptr_t(((GCHeap::kBlockSize<<(i-1))-1) & uintptr_t(item)) == 0", __FILE__, __LINE__);
          heap->Free(item);
       }


}
void create_mmgc_gcheap(AvmCore* core) { new ST_mmgc_gcheap(core); }
}
}
#endif

// Generated from ST_mmgc_gcoption.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_gcoption {
using namespace MMgc;

class ST_mmgc_gcoption : public Selftest {
public:
ST_mmgc_gcoption(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
    bool m_ret;
    bool m_wrong;
    GCHeap *m_heap;
    GCHeapConfig m_config_orig;
    bool isParamOption(const char* line) {
        return m_heap->config.IsGCOptionWithParam(line);
    }
    bool notParamOption(const char* line) {
        return !isParamOption(line);
    }
    bool approxEqual(double x, double y) {
        double delta1 = x - y;
        // abs() does not produce correct results at e.g. DBL_MAX.
        double delta2 = (delta1 > 0) ? delta1 : -delta1;
        return (delta2 < 0.0001);
    }
    void parseApply(const char* line, const char* optExtra=0)
    {
        m_ret = m_heap->config.ParseAndApplyOption(line, m_wrong, optExtra);
    }
    bool parsedCorrectly() {
        return m_ret && !m_wrong;
    }
    bool gcoptionButIncorrectFormat() {
        return m_ret && m_wrong;
    }

    void saveOrigHeapConfig() {
        memcpy(&m_config_orig, &m_heap->config, sizeof(GCHeapConfig));
    }

    void restoreHeapConfig() {
        memcpy(&m_heap->config, &m_config_orig, sizeof(GCHeapConfig));
    }

    bool configUnchanged() {
        const char* c1 = (const char*)&m_heap->config;
        const char* c2 = (const char*)&m_config_orig;
        bool noChangeSeen = true;
        for (size_t i=0; i < sizeof(GCHeapConfig); i++) {
            if (*c1 != *c2)
                noChangeSeen = false;
        }
        return noChangeSeen;
    }

};
ST_mmgc_gcoption::ST_mmgc_gcoption(AvmCore* core)
    : Selftest(core, "mmgc", "gcoption", ST_mmgc_gcoption::ST_names,ST_mmgc_gcoption::ST_explicits)
{}
const char* ST_mmgc_gcoption::ST_names[] = {"detect_parameterized_options","parse_memstats","parse_memlimit","parse_gcbehavior_gcsummary_eagersweep","parse_load_gcwork", NULL };
const bool ST_mmgc_gcoption::ST_explicits[] = {false,false,false,false,false, false };
void ST_mmgc_gcoption::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
}
}
void ST_mmgc_gcoption::prologue() {
{
    m_heap = GCHeap::GetGCHeap();
    saveOrigHeapConfig();
}

}
void ST_mmgc_gcoption::epilogue() {
{
    restoreHeapConfig();
}

}
void ST_mmgc_gcoption::test0() {
{
// line 75 "ST_mmgc_gcoption.st"
verifyPass(isParamOption("-memlimit"), "isParamOption(\"-memlimit\")", __FILE__, __LINE__);
// line 76 "ST_mmgc_gcoption.st"
verifyPass(isParamOption("-load"), "isParamOption(\"-load\")", __FILE__, __LINE__);
// line 77 "ST_mmgc_gcoption.st"
verifyPass(isParamOption("-loadCeiling"), "isParamOption(\"-loadCeiling\")", __FILE__, __LINE__);
// line 78 "ST_mmgc_gcoption.st"
verifyPass(isParamOption("-gcwork"), "isParamOption(\"-gcwork\")", __FILE__, __LINE__);
// line 79 "ST_mmgc_gcoption.st"
verifyPass(isParamOption("-gcstack"), "isParamOption(\"-gcstack\")", __FILE__, __LINE__);

// line 81 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-memlimit=10"), "notParamOption(\"-memlimit=10\")", __FILE__, __LINE__);
// line 82 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-load 1.5"), "notParamOption(\"-load 1.5\")", __FILE__, __LINE__);
// line 83 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-loadCeiling 1.5"), "notParamOption(\"-loadCeiling 1.5\")", __FILE__, __LINE__);
// line 84 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-gcwork 1.5"), "notParamOption(\"-gcwork 1.5\")", __FILE__, __LINE__);
// line 85 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-gcstack 10"), "notParamOption(\"-gcstack 10\")", __FILE__, __LINE__);

// line 87 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-not_an_option_and_never_will_be"), "notParamOption(\"-not_an_option_and_never_will_be\")", __FILE__, __LINE__);
// line 88 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-not_an_option_and_never_will_be 10"), "notParamOption(\"-not_an_option_and_never_will_be 10\")", __FILE__, __LINE__);
// line 89 "ST_mmgc_gcoption.st"
verifyPass(notParamOption("-not_an_option_and_never_will_be=10"), "notParamOption(\"-not_an_option_and_never_will_be=10\")", __FILE__, __LINE__);
          ;
}

}
void ST_mmgc_gcoption::test1() {
{
    // sanity checks:
    // - make sure our configUnchanged check is sane
    // - make sure our restoreHeapConfig works.
// line 98 "ST_mmgc_gcoption.st"
verifyPass(configUnchanged(), "configUnchanged()", __FILE__, __LINE__);
    memset(&m_heap->config, 0xfe, sizeof(GCHeapConfig));
// line 100 "ST_mmgc_gcoption.st"
verifyPass(!configUnchanged(), "!configUnchanged()", __FILE__, __LINE__);
    restoreHeapConfig();
// line 102 "ST_mmgc_gcoption.st"
verifyPass(configUnchanged(), "configUnchanged()", __FILE__, __LINE__);
          ;

    parseApply("-memstats");
// line 106 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 107 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.gcstats && m_heap->config.autoGCStats, "m_heap->config.gcstats && m_heap->config.autoGCStats", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-memstats-verbose");
// line 112 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 113 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.gcstats && m_heap->config.autoGCStats && m_heap->config.verbose, "m_heap->config.gcstats && m_heap->config.autoGCStats && m_heap->config.verbose", __FILE__, __LINE__);
          ;
    restoreHeapConfig();
}
}
void ST_mmgc_gcoption::test2() {
{
    parseApply("-memlimit   10");
// line 120 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 121 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.heapLimit == 10, "m_heap->config.heapLimit == 10", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-memlimit=11");
// line 126 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 127 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.heapLimit == 11, "m_heap->config.heapLimit == 11", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-memlimit = 12");
// line 132 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 133 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.heapLimit == 12, "m_heap->config.heapLimit == 12", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-memlimit");
// line 138 "ST_mmgc_gcoption.st"
verifyPass(gcoptionButIncorrectFormat(), "gcoptionButIncorrectFormat()", __FILE__, __LINE__);
// line 139 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.heapLimit == m_config_orig.heapLimit, "m_heap->config.heapLimit == m_config_orig.heapLimit", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-memlimit", "13");
// line 144 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 145 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.heapLimit == 13, "m_heap->config.heapLimit == 13", __FILE__, __LINE__);
          ;
    restoreHeapConfig();
}
}
void ST_mmgc_gcoption::test3() {
{
#ifdef MMGC_POLICY_PROFILING
    parseApply("-gcbehavior");
// line 153 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 154 "ST_mmgc_gcoption.st"
verifyPass((m_heap->config.gcbehavior == 2), "(m_heap->config.gcbehavior == 2)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-gcsummary");
// line 159 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 160 "ST_mmgc_gcoption.st"
verifyPass((m_heap->config.gcbehavior == 1), "(m_heap->config.gcbehavior == 1)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();
#endif

    parseApply("-eagersweep");
// line 166 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 167 "ST_mmgc_gcoption.st"
verifyPass(m_heap->config.eagerSweeping, "m_heap->config.eagerSweeping", __FILE__, __LINE__);
          ;
    restoreHeapConfig();
}
}
void ST_mmgc_gcoption::test4() {
{
    parseApply("-load 7.0");
// line 174 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 175 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[0], 7.0), "approxEqual(m_heap->config.gcLoad[0], 7.0)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    // test load with '<space><param>'
    parseApply("-load 6.0,10,5.0");
// line 181 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 182 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[0], 6.0), "approxEqual(m_heap->config.gcLoad[0], 6.0)", __FILE__, __LINE__);
// line 183 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoadCutoff[0], 10.0), "approxEqual(m_heap->config.gcLoadCutoff[0], 10.0)", __FILE__, __LINE__);
// line 184 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[1], 5.0), "approxEqual(m_heap->config.gcLoad[1], 5.0)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    // test load with separate <param>
    parseApply("-load", "8.0,20.5,7.0");
// line 190 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 191 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[0], 8.0), "approxEqual(m_heap->config.gcLoad[0], 8.0)", __FILE__, __LINE__);
// line 192 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoadCutoff[0], 20.5), "approxEqual(m_heap->config.gcLoadCutoff[0], 20.5)", __FILE__, __LINE__);
// line 193 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[1], 7.0), "approxEqual(m_heap->config.gcLoad[1], 7.0)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    // test load with '=<param>'
    parseApply("-load=10.0,30.5,9.0");
// line 199 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 200 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[0], 10.0), "approxEqual(m_heap->config.gcLoad[0], 10.0)", __FILE__, __LINE__);
// line 201 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoadCutoff[0], 30.5), "approxEqual(m_heap->config.gcLoadCutoff[0], 30.5)", __FILE__, __LINE__);
// line 202 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[1], 9.0), "approxEqual(m_heap->config.gcLoad[1], 9.0)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    // Max load pairs is 7
    parseApply("-load 1.5,1.5,2,2,3,3,4,4,5,5,6,6,7,7,8,8");
// line 208 "ST_mmgc_gcoption.st"
verifyPass(gcoptionButIncorrectFormat(), "gcoptionButIncorrectFormat()", __FILE__, __LINE__);
// line 209 "ST_mmgc_gcoption.st"
verifyPass(configUnchanged(), "configUnchanged()", __FILE__, __LINE__);
    restoreHeapConfig();

    // Ensure that the last load value is ignored
    parseApply("-load=10.0,30.0,9.0,60.0");
// line 214 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 215 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[0], 10.0), "approxEqual(m_heap->config.gcLoad[0], 10.0)", __FILE__, __LINE__);
// line 216 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoadCutoff[0], 30.0), "approxEqual(m_heap->config.gcLoadCutoff[0], 30.0)", __FILE__, __LINE__);
// line 217 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoad[1], 9.0), "approxEqual(m_heap->config.gcLoad[1], 9.0)", __FILE__, __LINE__);
// line 218 "ST_mmgc_gcoption.st"
verifyPass(!approxEqual(m_heap->config.gcLoadCutoff[1], 60.0), "!approxEqual(m_heap->config.gcLoadCutoff[1], 60.0)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-load");
// line 223 "ST_mmgc_gcoption.st"
verifyPass(gcoptionButIncorrectFormat(), "gcoptionButIncorrectFormat()", __FILE__, __LINE__);
// line 224 "ST_mmgc_gcoption.st"
verifyPass(configUnchanged(), "configUnchanged()", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    // L (load) must be > 1
    parseApply("-load 1,30");
// line 230 "ST_mmgc_gcoption.st"
verifyPass(gcoptionButIncorrectFormat(), "gcoptionButIncorrectFormat()", __FILE__, __LINE__);
// line 231 "ST_mmgc_gcoption.st"
verifyPass(configUnchanged(), "configUnchanged()", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-load badvalue");
// line 236 "ST_mmgc_gcoption.st"
verifyPass(gcoptionButIncorrectFormat(), "gcoptionButIncorrectFormat()", __FILE__, __LINE__);
// line 237 "ST_mmgc_gcoption.st"
verifyPass(configUnchanged(), "configUnchanged()", __FILE__, __LINE__);
          ;
    restoreHeapConfig();


    parseApply("-loadCeiling 11.5");
// line 243 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 244 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcLoadCeiling, 11.5), "approxEqual(m_heap->config.gcLoadCeiling, 11.5)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-gcwork 12.5");
// line 249 "ST_mmgc_gcoption.st"
verifyPass(gcoptionButIncorrectFormat(), "gcoptionButIncorrectFormat()", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-gcwork 0.123456");
// line 254 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 255 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcEfficiency, .123456), "approxEqual(m_heap->config.gcEfficiency, .123456)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-gcwork=0.23456");
// line 260 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 261 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcEfficiency, .23456), "approxEqual(m_heap->config.gcEfficiency, .23456)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

    parseApply("-gcwork", "0.3456");
// line 266 "ST_mmgc_gcoption.st"
verifyPass(parsedCorrectly(), "parsedCorrectly()", __FILE__, __LINE__);
// line 267 "ST_mmgc_gcoption.st"
verifyPass(approxEqual(m_heap->config.gcEfficiency, .3456), "approxEqual(m_heap->config.gcEfficiency, .3456)", __FILE__, __LINE__);
          ;
    restoreHeapConfig();

}

}
void create_mmgc_gcoption(AvmCore* core) { new ST_mmgc_gcoption(core); }
}
}
#endif

// Generated from ST_mmgc_mmfx_array.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_mmfx_array {
class ST_mmgc_mmfx_array : public Selftest {
public:
ST_mmgc_mmfx_array(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
};
ST_mmgc_mmfx_array::ST_mmgc_mmfx_array(AvmCore* core)
    : Selftest(core, "mmgc", "mmfx_array", ST_mmgc_mmfx_array::ST_names,ST_mmgc_mmfx_array::ST_explicits)
{}
const char* ST_mmgc_mmfx_array::ST_names[] = {"mmfx_new_array_oom","mmfx_new_array_opts_canFail","mmfx_new_array_opts_oom", NULL };
const bool ST_mmgc_mmfx_array::ST_explicits[] = {true,true,true, false };
void ST_mmgc_mmfx_array::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
}
}
using namespace MMgc;

// There are different failure modes.
// 1) if you call mmfx_new_array() it can OOM:
//     -> it should OOM and shutdown
// 2) if you call mmfx_new_array_opts(canFail) and it OOMs:
//     -> it should return NULL and NOT OOM
// 3) if you call mmfx_new_aray_opts() without the canFail flag:
//     -> it should OOM and shutdown


void ST_mmgc_mmfx_array::test0() {
    // This will cause an OOM abort and exit, run with -memlimit 1024
    int* m_ints = mmfx_new_array(int, 1048576);
    mmfx_delete_array(m_ints);
    m_ints = NULL;
// line 27 "ST_mmgc_mmfx_array.st"
verifyPass(false, "false", __FILE__, __LINE__);

}
void ST_mmgc_mmfx_array::test1() {
    // This should fail and return NULL, run with -memlimit 1024
    int* m_ints = mmfx_new_array_opt(int, 1048576, MMgc::kCanFail);
// line 32 "ST_mmgc_mmfx_array.st"
verifyPass(m_ints == NULL, "m_ints == NULL", __FILE__, __LINE__);
    mmfx_delete_array(m_ints);
    m_ints = NULL;

}
void ST_mmgc_mmfx_array::test2() {
    // This will cause an OOM abort and exit, run with -memlimit 1024
    int* m_ints = mmfx_new_array_opt(int, 1048576, MMgc::kZero);
    mmfx_delete_array(m_ints);
    m_ints = NULL;
// line 41 "ST_mmgc_mmfx_array.st"
verifyPass(false, "false", __FILE__, __LINE__);



}
void create_mmgc_mmfx_array(AvmCore* core) { new ST_mmgc_mmfx_array(core); }
}
}
#endif

// Generated from ST_mmgc_threads.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if defined VMCFG_WORKERTHREADS
namespace avmplus {
namespace ST_mmgc_threads {
class ST_mmgc_threads : public Selftest {
public:
ST_mmgc_threads(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();

private:
    MMgc::GC *gc;
    MMgc::FixedAlloc *fa;
    MMgc::FixedMalloc *fm;
    bool waiting;
    bool result;
    bool isDead;
    pthread_t pthread;
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;

    static void* slaveRunner(void *arg)
    {
        ((ST_mmgc_threads*)arg)->slaveRun();
        return NULL;
    }

    void slaveRun()
    {
        wait();
        {
      MMGC_ENTER_VOID;
         MMGC_GCENTER(gc);
         result &= !isDead;
         gc->ReapZCT();
         result &= !isDead;
         gc->Collect();
         result &= !isDead;
        }
        kick();
    }

    void startSlave()
    {
       pthread_create(&pthread, NULL, slaveRunner, this);
    }

    void kick()
    {
        pthread_mutex_lock (&pmutex);
        while(!waiting) {
            pthread_mutex_unlock (&pmutex);
            usleep(100);
            pthread_mutex_lock (&pmutex);
        }
        pthread_cond_signal (&pcond);
        while(waiting) {
            pthread_mutex_unlock (&pmutex);
            usleep(100);
            pthread_mutex_lock (&pmutex);
        }
        pthread_mutex_unlock (&pmutex);
    }

    void wait()
    {
        pthread_mutex_lock (&pmutex);
        GCAssert(waiting == false);
        waiting = true;
        pthread_cond_wait (&pcond, &pmutex);
        waiting = false;
        pthread_mutex_unlock (&pmutex);
    }

    static void kickAndWait(void* arg)
    {
        ST_mmgc_threads* self = (ST_mmgc_threads*)arg;
        self->kick();
        self->wait();
    }

};
ST_mmgc_threads::ST_mmgc_threads(AvmCore* core)
    : Selftest(core, "mmgc", "threads", ST_mmgc_threads::ST_names,ST_mmgc_threads::ST_explicits)
{}
const char* ST_mmgc_threads::ST_names[] = {"mmgc_gc_root_thread", NULL };
const bool ST_mmgc_threads::ST_explicits[] = {false, false };
void ST_mmgc_threads::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_mmgc_threads::prologue() {
    MMgc::GCConfig config;
    gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    if (gc==NULL) {
        MMgc::GCHeap::Init();
        gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    }
    pthread_mutex_init(&pmutex, NULL);
    pthread_cond_init(&pcond, NULL);
    result = true;
    isDead = false;
    waiting = false;

}
void ST_mmgc_threads::epilogue() {
    pthread_mutex_destroy(&pmutex);
    pthread_cond_destroy(&pcond);
    delete gc;

}
using namespace MMgc;

class RCObjectNotifier : public RCObject
{
public:
        RCObjectNotifier(bool *isDead) : isDead(isDead) {}
        ~RCObjectNotifier() { *isDead = true; isDead = NULL; }
        bool *isDead;
};

void ST_mmgc_threads::test0() {
       startSlave();
       MMGC_GCENTER(gc);
       RCObjectNotifier *obj = new (gc) RCObjectNotifier(&isDead);
       {
          gc->CreateRootFromCurrentStack(kickAndWait, this);
       }

// line 122 "ST_mmgc_threads.st"
verifyPass(result, "result", __FILE__, __LINE__);

// line 124 "ST_mmgc_threads.st"
verifyPass(!isDead, "!isDead", __FILE__, __LINE__);
       gc->ReapZCT();
// line 126 "ST_mmgc_threads.st"
verifyPass(!isDead, "!isDead", __FILE__, __LINE__);
       gc->Collect();
// line 128 "ST_mmgc_threads.st"
verifyPass(!isDead, "!isDead", __FILE__, __LINE__);

       pthread_join(pthread, NULL);

       printf("Ignore this: %d\n", *obj->isDead);

}
void create_mmgc_threads(AvmCore* core) { new ST_mmgc_threads(core); }
}
}
#endif
#endif

// Generated from ST_mmgc_weakref.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_mmgc_weakref {
using namespace MMgc;

class C : public GCFinalizedObject
{
public:
    C(int key) : key(key) {}
    ~C() { key = -1; }
    int key;
    GCMember<C> next;
};

class MyCallback : public GCCallback
{
public:
    MyCallback(GC* gc, GCWeakRef** refs, C** objs, int n)
        : GCCallback(gc)
        , refs(refs)
        , objs(objs)
        , n(n)
    {
    }

    virtual void presweep() {
    for ( int i=1 ; i < 1000 ; i+=2 )
        objs[i]->next = (C*)(void *)(refs[i-1]->get());
    }

private:
    GCWeakRef** refs;
    C** objs;
    int n;
};

class D : public GCFinalizedObject
{
public:
    ~D() { GC::GetWeakRef(this); }
};

class ST_mmgc_weakref : public Selftest {
public:
ST_mmgc_weakref(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
};
ST_mmgc_weakref::ST_mmgc_weakref(AvmCore* core)
    : Selftest(core, "mmgc", "weakref", ST_mmgc_weakref::ST_names,ST_mmgc_weakref::ST_explicits)
{}
const char* ST_mmgc_weakref::ST_names[] = {"unmarked_object_presweep","unmarked_object_finalize", NULL };
const bool ST_mmgc_weakref::ST_explicits[] = {false,true, false };
void ST_mmgc_weakref::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
}
}
void ST_mmgc_weakref::test0() {

  // Bugzilla 572331 - it's possible for a weak reference to yield up a pointer to an unmarked
  // object in a presweep handler, and for that pointer to be stored into the heap even with
  // a WB macro, without the barrier triggering.

{
    GC* gc = core->gc;

    // Stack allocated storage, so automatically these arrays are roots

    GCWeakRef* refs[1000];
    C* objs[1000];

    // Create weak and strong references to 1000 objects

    for ( int i=0 ; i < 1000 ; i++ ) {
        objs[i] = new (gc) C(i);
        refs[i] = objs[i]->GetWeakRef();
    }

    // Get the collector into a reasonable state.

    gc->Collect();
    gc->Collect();

    // Remove the even-numbered strong refs.

    for ( int i=0 ; i < 1000 ; i+=2 )
        objs[i] = NULL;

    // Introduce mischief.  The presweep callback will extract
    // pointers from the even-numbered weak refs and insert them into
    // the objects in the odd-numbered slots (which are strongly
    // held).  Most of the objects referenced from the even-numbered
    // weak refs should be weakly held and unmarked.  We hope to catch
    // the collector picking up an unmarked object.

    MyCallback* cb = new MyCallback(gc, refs, objs, 1000);

    // Now trigger the collector again.

    gc->Collect();

    // Prevent more mischief from happening.

    delete cb;

    // Now reference all the odd-numbered objects and check the
    // integrity of their 'next' objects.  There's a chance this may
    // crash.

    for ( int i=1 ; i < 1000 ; i+= 2 ) {
// line 104 "ST_mmgc_weakref.st"
verifyPass(objs[i]->next->key == i-1, "objs[i]->next->key == i-1", __FILE__, __LINE__);
    }

    // Clean up
    VMPI_memset(refs, 0, sizeof(refs));
    VMPI_memset(objs, 0, sizeof(objs));
}

}
void ST_mmgc_weakref::test1() {

  // Bugzilla 647155 - ditto as the previous test, but now it's the object's destructor that tries
  // to store a pointer to an unmarked object (in this case the object itself) into a weak ref.
  // This will assert in debug builds, so the test is marked "explicit" for that reason: in a
  // debug build we want to verify that the assert is hit, in a release build we want to verify that
  // the test does not crash.
  
{
    GC* gc = core->gc;

    // Stack allocated storage, so automatically these arrays are roots

    D* objs[1000];

    // Create strong references to 1000 objects

    for ( int i=0 ; i < 1000 ; i++ ) {
        objs[i] = new (gc) D();
    }

    // Get the collector into a reasonable state.

    gc->Collect();
    gc->Collect();

    // Remove the even-numbered strong refs.

    for ( int i=0 ; i < 1000 ; i+=2 )
        objs[i] = NULL;

    // Now trigger the collector again.

    gc->Collect();

// line 147 "ST_mmgc_weakref.st"
verifyPass(true, "true", __FILE__, __LINE__);
}

}
void create_mmgc_weakref(AvmCore* core) { new ST_mmgc_weakref(core); }
}
}
#endif

// Generated from ST_nanojit_codealloc.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_nanojit_codealloc {

// This conditionalization is clumsy, but we don't seem to have any other
// mechanism to enable a selftest conditionally upon the configuration.

#ifdef VMCFG_NANOJIT

using nanojit::NIns;
using nanojit::CodeList;
using nanojit::CodeAlloc;

static bool verbose = false;

class Randomizer {
private:

    TRandomFast seed;

public:

    Randomizer()
    {
        MathUtils::initRandom(&seed);
    }
    
    // Return integer in range [min,max).

    uint32_t range(uint32_t min, uint32_t max)
    {
        return min + MathUtils::Random(max - min, &seed);
    }
};

class CodeAllocDriver {
public:

    struct CodeListProfile {
        uint32_t largefreq;     // 1 in 'largefreq' chance codelist will be "large"
        uint32_t smallmin;      // Minimum size of "small" codelist
        uint32_t smallmax;      // Maximum size of "small" codelist
        uint32_t largemin;      // Minimum size of "large" codelist
        uint32_t largemax;      // Maximum size of "large" coelist
    };

    struct AllocationProfile {
        uint32_t limit;         // Largest block allocation permitted, zero for no limit
        uint32_t leftfreq;      // 1 in 'leftfreq' chance returned space will start at beginning of block
    };

    CodeAllocDriver(uint32_t max_codelists, CodeListProfile& codelist_profile, AllocationProfile& alloc_profile);
    
    void run(uint32_t iterations);
        
private:

    Randomizer rand;
    uint32_t n_active;          // Maximum number of simultaneously-allocated codelists
    CodeListProfile cp;         // Parameters controlling distribution of codelist sizes
    AllocationProfile ap;       // Parameters controlling allocation of each codelist

    void makeCodeList(uint32_t index, CodeAlloc& alloc, CodeList*& code);
};

CodeAllocDriver::CodeAllocDriver(uint32_t max_codelists, CodeListProfile& codelist_profile, AllocationProfile& alloc_profile)
    : n_active(max_codelists), cp(codelist_profile), ap(alloc_profile)
{}

// Create a codelist at the specified index in the codelist pool.
// The length of the codelist is generated pseudo-randomly.

void CodeAllocDriver::makeCodeList(uint32_t index, CodeAlloc& alloc, CodeList*& code)
{
     NIns* start;
     NIns* end;
  
     int32_t size = ((rand.range(0, cp.largefreq) > 0)
                     ? rand.range(cp.smallmin, cp.smallmax)
                     : rand.range(cp.largemin, cp.largemax));
     if (verbose)
         AvmLog("code-heap-test: codelist %d size %d\n", index, size);
     while (size > 0) {
         alloc.alloc(start, end, ap.limit);
         uint32_t blksize = uint32_t(end - start);
         if (verbose)
             AvmLog("code-heap-test: alloc  %x-%x %d\n", start, start, blksize);
         size -= blksize;
         if (size < 0) {
             // Last allocated block was not completely used, so we will return some of it.
             // We choose randomly whether to return a prefix of the block, or a segment out
             // of the middle.  This doesn't correspond precisely to normal usage in nanojit.
             // Note that if we are not retaining at least two bytes, we cannot split in the middle.
             int32_t excess = -size;
             if (rand.range(0, ap.leftfreq) > 0 || blksize - excess < 2) {
                 // Hole starts at left
                 // Note that hole at right is deliberately not handled in CodegenAlloc.cpp, results in assert.
                 if (verbose)
                     AvmLog("code-heap-test: hole l %x-%x %d\n", start, start + excess, excess);
                 alloc.addRemainder(code, start, end, start, start + excess);
             } else {
                 // Hole will go in middle
                 uint32_t offset = rand.range(1, blksize - excess);
                 if (verbose)
                     AvmLog("code-heap-test: hole m %x-%x %d\n", start + offset, start + offset + excess, excess);
                 alloc.addRemainder(code, start, end, start + offset, start + offset + excess);
             }
         } else {
             // Add entire block to codelist.
             CodeAlloc::add(code, start, end);
         }
     }
}

// Repeatedly construct and destroy codelists.
// We maintain a pool of codelists.  On each iteration, we replace one
// of the existing codelists with a new one, freeing the old.

void CodeAllocDriver::run(uint32_t iterations)
{
    nanojit::Config config;
    // Enable page protection flag checking during test code
    config.check_page_flags = true;

    CodeAlloc alloc(&config);

    CodeList** codelists = mmfx_new_array(CodeList*, n_active);

    VMPI_memset(codelists, 0, n_active * sizeof(CodeList*));

    for (uint32_t i = 0; i < iterations; i++) {
        uint32_t victim = rand.range(0, n_active);
        alloc.freeAll(codelists[victim]);
        makeCodeList(victim, alloc, codelists[victim]);
        //alloc.logStats();
        debug_only( alloc.sanity_check(); )
    }

    mmfx_delete_array(codelists);
}

#endif /* VMCFG_NANOJIT */

class ST_nanojit_codealloc : public Selftest {
public:
ST_nanojit_codealloc(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
};
ST_nanojit_codealloc::ST_nanojit_codealloc(AvmCore* core)
    : Selftest(core, "nanojit", "codealloc", ST_nanojit_codealloc::ST_names,ST_nanojit_codealloc::ST_explicits)
{}
const char* ST_nanojit_codealloc::ST_names[] = {"allocfree", NULL };
const bool ST_nanojit_codealloc::ST_explicits[] = {false, false };
void ST_nanojit_codealloc::run(int n) {
switch(n) {
case 0: test0(); return;
}
}

#ifdef VMCFG_NANOJIT

typedef CodeAllocDriver::CodeListProfile CodeListProfile;
typedef CodeAllocDriver::AllocationProfile AllocationProfile;

static CodeListProfile cp = { 5, 1, 1*1024, 1, 16*1024 };

static AllocationProfile ap[] = { { 0,     2 },
                                  #ifndef NANOJIT_64BIT
                                  // 128-byte minimum is too small on 64-bit platorms.
                                  { 128,   2 },
                                  #endif
                                  { 512,   2 },
                                  { 1024,  2 },
                                  { 2048,  2 },
                                  { 4096,  2 } };

static uint32_t n_ap = sizeof(ap) / sizeof(AllocationProfile);

#endif /* VMCFG_NANOJIT */

void ST_nanojit_codealloc::test0() {

#ifdef VMCFG_NANOJIT

for (uint32_t i = 0; i < n_ap; i++) {
    CodeAllocDriver* driver = mmfx_new(CodeAllocDriver(20, cp, ap[i]));
#if defined(DEBUG) && !(defined(VMCFG_IA32) || defined(VMCFG_AMD64))
    // This test is very slow in debug builds, due to the calls to sanity_check().
    // Run an abbreviated version of the test except on desktop platforms.
    driver->run(200);
#else
    driver->run(20000);
#endif
    mmfx_delete(driver);
 }

#endif /* VMCFG_NANOJIT */

// We pass if we don't crash or assert.
// line 194 "ST_nanojit_codealloc.st"
verifyPass(true, "true", __FILE__, __LINE__);


}
void create_nanojit_codealloc(AvmCore* core) { new ST_nanojit_codealloc(core); }
}
}
#endif

// Generated from ST_vmbase_concurrency.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_vmbase_concurrency {
using namespace MMgc;
using namespace vmbase;

// We use the same testing method as that for ST_vmpi_threads:
// Each construct is tested by (1) using it in the implementation of a mutator that
// modifies a counter for a fixed number of iterations, and then (2) running
// duplicates of that mutator in parallel. The final counter value ends up in
// sharedCounter, which is guarded by m_monitor (except for in CASTest).
// Each test checks that the sharedCounter ends up with a statically determined
// end value.
// (Description courtesy of Felix)

// We need a specific namespace as ST_vmpi_threads uses the same test class names
namespace selftestconcurrency {

    class ThreadTestBase : public Runnable {
        public:
            ThreadTestBase(int iterations) : m_iterations(iterations), sharedCounter(0) {
            }
            virtual ~ThreadTestBase() {
            }

        protected:
            WaitNotifyMonitor m_monitor;
            const int m_iterations;

        public:
            int sharedCounter;
    };

    class TestRunner {
        public:
            struct ThreadRecord {
                VMThread* thread;
                bool startupOk;
            };

            TestRunner(int threadQty, bool doJoin = true) : m_threadQty(threadQty), m_doJoin(doJoin) {
            }
            virtual ~TestRunner() {
            }

            void runTest(ThreadTestBase& test) {

                m_threads = mmfx_new_array(ThreadRecord, m_threadQty);

                // Start up the threads
                for (int i = 0; i < m_threadQty; i++) {
                    m_threads[i].thread = mmfx_new(VMThread(&test));
                    m_threads[i].startupOk = m_threads[i].thread->start();
                }

                // ...then block until they all terminate
                for (int i = 0; i < m_threadQty; i++) {
                    if (m_doJoin && m_threads[i].startupOk) {
                        m_threads[i].thread->join();
                    }
                    mmfx_delete(m_threads[i].thread);
                }

                mmfx_delete_array(m_threads);
            }

        private:
            const int m_threadQty;
            ThreadRecord* m_threads;
            bool m_doJoin;
    };

    class MutexTest : public ThreadTestBase {
        public:
            MutexTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~MutexTest() {}

            virtual void run() {
                AvmAssert(m_iterations % 2 == 0);
                for (int i = 0; i < m_iterations/2; i++) {
                    SCOPE_LOCK(m_monitor) {
                        SCOPE_LOCK(m_monitor) {
                            sharedCounter++;
                        }
                    }
                }
                for (int i = 0; i < m_iterations/2; i++) {
                    SCOPE_LOCK_NAMED(locker, m_monitor) {
                        SCOPE_LOCK_NAMED(locker, m_monitor) {
                            sharedCounter++;
                        }
                    }
                }
            }
    };
    class ConditionTest : public ThreadTestBase {
        public:
            ConditionTest(int iterations, int threadQty) : ThreadTestBase(iterations), m_threadQty(threadQty) {}
            virtual ~ConditionTest() {}

            virtual void run() {

                AvmAssert(m_threadQty >= 2);

                for (int i = 0; i < m_iterations; i++) {
                    SCOPE_LOCK_NAMED(locker, m_monitor) {
                        sharedCounter++;
                        // If there's another thread still active then wait.
                        if (m_threadQty > 1) {
                            locker.notify();
                            locker.wait();
                        }
                        // This thread has finished, so let's wake everyone else up
                        if (i == m_iterations - 1) {
                            --m_threadQty;
                            locker.notifyAll();
                        }
                    }

                }

            }

        private:
            int m_threadQty;
    };

    class AtomicCounterTest : public ThreadTestBase {
        public:
            AtomicCounterTest(int iterations, int threadQty) : ThreadTestBase(iterations), m_threadQty(threadQty) {}
            virtual ~AtomicCounterTest() {}

            virtual void run() {

                AvmAssert(m_iterations % 4 == 0);

                for (int i = 0; i < m_iterations/4; i++) {
                    m_counter++;
                }
                for (int i = 0; i < m_iterations/4; i++) {
                    m_counter--;
                }
                for (int i = 0; i < m_iterations/4; i++) {
                    ++m_counter;
                }
                for (int i = 0; i < m_iterations/4; i++) {
                    --m_counter;
                }

                SCOPE_LOCK(m_monitor) {
                    if (--m_threadQty == 0) {
                        sharedCounter = m_counter;
                    }
                }
            }
        private:
            AtomicCounter32 m_counter;
            int m_threadQty;
    };

    class CASTest : public ThreadTestBase {
        public:
            CASTest(int iterations, bool withBarrier) : ThreadTestBase(iterations), m_withBarrier(withBarrier) {}
            virtual ~CASTest() {}

            virtual void run() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!AtomicOps::compareAndSwap32WithBarrier(current, next, &sharedCounter));
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!AtomicOps::compareAndSwap32(current, next, &sharedCounter));
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    /**
     * We protect a shared counter with a Dekker-style lock that has been made
     * sequentially consistent with memory barriers.
     *
     * The idea is that if the barriers are correct, then two threads can compete
     * to update the counter n times each, so that the final counter value is 2n. If
     * the final value is not 2n, then the barriers have failed to ensure sequential
     * consistency.
     *
     * FIXME: bug 609943
     * This seems way too complicated. We have to be confident in the algorithm
     * before considering the barrier implementations, and I'm not convinced as yet.
     * Is there something simpler?
     * Note that the barriers below are extremely conservative.
     *
     * This is test is not actually run. The verifyPass below just returns true.
     */
    class MemoryBarrierTest : public ThreadTestBase {
        public:
            MemoryBarrierTest(int iterations) : ThreadTestBase(iterations), m_thread0(0), m_thread1(0), m_turn(NULL) {}
            virtual ~MemoryBarrierTest() {}

            virtual void run() {

                volatile int* me;
                volatile int* other;
                volatile int* const counterp = &sharedCounter;

                SCOPE_LOCK(m_monitor) {
                    if (m_turn == NULL) {
                        me = &m_thread0;
                        other = &m_thread1;
                        m_turn = me;
                    } else {
                        me = &m_thread1;
                        other = &m_thread0;
                    }
                }

                for (int i = 0; i < m_iterations; i++) {
                    // Dekker lock
                    *me = 1;
                    MemoryBarrier::readWrite();
                    while (*other == 1) {
                        MemoryBarrier::readWrite();
                        if (m_turn == other) {
                            MemoryBarrier::readWrite();
                            *me = 0;
                            MemoryBarrier::readWrite();
                            while (m_turn == other) {
                                MemoryBarrier::readWrite();
                            }
                            MemoryBarrier::readWrite();
                            *me = 1;
                            MemoryBarrier::readWrite();
                        }
                    }
                    MemoryBarrier::readWrite();
                    (*counterp)++;
                    MemoryBarrier::readWrite();
                    m_turn = other;
                    MemoryBarrier::readWrite();
                    *me = 0;
                    MemoryBarrier::readWrite();
                }
            }
        private:
            volatile int m_thread0;
            volatile int m_thread1;
            volatile int* volatile m_turn;
    };

    class ConditionWithWaitTest : public ThreadTestBase {
        public:
            ConditionWithWaitTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~ConditionWithWaitTest() {}

            virtual void run() {
                for (int i = 0; i < m_iterations; i++) {
                    SCOPE_LOCK_NAMED(locker, m_monitor) {
                        sharedCounter++;
                        locker.wait(1);
                    }
                }
            }
    };

    class SleepTest : public ThreadTestBase {
        public:
            SleepTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~SleepTest() {}

            virtual void run() {
                for (int i = 0; i < m_iterations; i++) {
                    SCOPE_LOCK(m_monitor) {
                        sharedCounter++;
                    }
                    VMThread::sleep(1);
                }
            }
    };

    class VMThreadLocalTest : public ThreadTestBase {
        public:
            VMThreadLocalTest(int iterations) : ThreadTestBase(iterations) {}
            virtual ~VMThreadLocalTest() {}

            virtual void run() {
                for (int i = 0; i < m_iterations; i++) {
                    m_localCounter.set(m_localCounter.get() + 1);
                }
                SCOPE_LOCK(m_monitor) {
                    sharedCounter += (int)m_localCounter;
                }
            }
        private:
            VMThreadLocal<uintptr_t> m_localCounter;
    };
}

// This needs to be at least 2 for ConditionTest
#define THREAD_QTY 4
#define ITERATIONS 100000

using namespace selftestconcurrency;

class ST_vmbase_concurrency : public Selftest {
public:
ST_vmbase_concurrency(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();
void test9();
};
ST_vmbase_concurrency::ST_vmbase_concurrency(AvmCore* core)
    : Selftest(core, "vmbase", "concurrency", ST_vmbase_concurrency::ST_names,ST_vmbase_concurrency::ST_explicits)
{}
const char* ST_vmbase_concurrency::ST_names[] = {"mutexes","conditions","atomic_counter","compare_and_swap_without_barrier","compare_and_swap_with_barrier","memory_barrier","condition_with_wait","sleep","vmthreadlocal","join", NULL };
const bool ST_vmbase_concurrency::ST_explicits[] = {false,false,false,false,false,false,false,false,false,false, false };
void ST_vmbase_concurrency::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
case 5: test5(); return;
case 6: test6(); return;
case 7: test7(); return;
case 8: test8(); return;
case 9: test9(); return;
}
}
void ST_vmbase_concurrency::test0() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    MutexTest test(ITERATIONS);
    runner.runTest(test);
// line 328 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test1() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    ConditionTest test(ITERATIONS, THREAD_QTY);
    runner.runTest(test);
// line 336 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test2() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    AtomicCounterTest test(ITERATIONS, THREAD_QTY);
    runner.runTest(test);
// line 344 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == 0, "test.sharedCounter == 0", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test3() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    CASTest test(ITERATIONS, false);
    runner.runTest(test);
// line 352 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test4() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    CASTest test(ITERATIONS, true);
    runner.runTest(test);
// line 360 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test5() {
#ifndef UNDER_CE
    /* This test is failing on Windows and Mac OSX 10.4.
     * For Windows, see bug 609820.
     * For Mac, are the 10.4 APIs not reliable?
     * It could also be the test, or the compiler!
     * FIXME: bug 609943 Selftests to stress memory barriers (fences)

    // Note that the memory barrier test is based on a Dekker lock, so we
    // only ever use 2 threads.
    TestRunner runner(2);
    MemoryBarrierTest test(ITERATIONS);
    runner.runTest(test);
// line 376 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == 2 * ITERATIONS, "test.sharedCounter == 2 * ITERATIONS", __FILE__, __LINE__);
    */
    
// line 379 "ST_vmbase_concurrency.st"
verifyPass(true, "true", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test6() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    ConditionWithWaitTest test(2000); // Use 2000 iterations with a 1 ms wait
    runner.runTest(test);
// line 387 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * 2000, "test.sharedCounter == THREAD_QTY * 2000", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test7() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    SleepTest test(2000); // Use 2000 iterations with a 1 ms sleep
    runner.runTest(test);
// line 395 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * 2000, "test.sharedCounter == THREAD_QTY * 2000", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test8() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY);
    VMThreadLocalTest test(ITERATIONS);
    runner.runTest(test);
// line 403 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_concurrency::test9() {
#ifndef UNDER_CE
    // We should be able to run the dtor of a non-started VMThread.
    {
        VMThread vmthread;
    }
    // Run the mutex test but call the VMThread dtors without joining first
    TestRunner runner(THREAD_QTY, false);
    MutexTest test(ITERATIONS);
    runner.runTest(test);
// line 416 "ST_vmbase_concurrency.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif





}
void create_vmbase_concurrency(AvmCore* core) { new ST_vmbase_concurrency(core); }
}
}
#endif

// Generated from ST_vmbase_safepoints.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if defined VMCFG_SAFEPOINTS && defined BUG_754918
namespace avmplus {
namespace ST_vmbase_safepoints {
using namespace MMgc;
using namespace vmbase;

    /**
     * Base class for tests run by a TestRunner.
     * Sub-classes should override runInSafepointManagerContext().
     */
    class SafepointTestBase : public Runnable
    {
    public:
        SafepointTestBase(SafepointManager& safepointManager, int iterations, int threadQty)
            : m_safepointManager(safepointManager)
            , m_iterations(iterations)
            , m_threadQty(threadQty)
            , sharedCounter(0)
        {
        }

        virtual ~SafepointTestBase()
        {
        }

        virtual void run()
        {
            // Enter the safepoint context shared by the primordial and worker threads
            SafepointRecord spRecord;
            m_safepointManager.enter(&spRecord);

            runInSafepointManagerContext();

            // Cleanup
            m_safepointManager.leave(&spRecord);
        }

        // Tests should override this function
        virtual void runInSafepointManagerContext() = 0;

        int threadQty()
        {
            return m_threadQty;
        }

    protected:
        SafepointManager& m_safepointManager;
        WaitNotifyMonitor m_monitor;
        const int m_iterations;
        const int m_threadQty;

    public:
        int sharedCounter;
    };

    /**
     * Creates the given number of VMThreads, and passes each the SafepointTestBase to execute.
     * The TestRunner contains the SafepointManager with which the primordial and each worker
     * thread will register their topmost SafepointRecord.
     */
    class TestRunner
    {
    public:
        struct ThreadRecord
        {
            VMThread* thread;
            bool startupOk;
        };

        TestRunner(bool doJoin = true)
            : m_doJoin(doJoin)
        {
        }

        virtual ~TestRunner()
        {
        }

        void runTest(SafepointTestBase& test)
        {
            // Enter the safepoint context shared by the primordial and worker threads
            SafepointRecord spRecord;
            m_safepointManager.enter(&spRecord);

            m_threads = mmfx_new_array(ThreadRecord, test.threadQty());

            // Start up the threads.
            // Each should also enter m_safepointManager
            for (int i = 0; i < test.threadQty(); i++) {
                m_threads[i].thread = mmfx_new(VMThread(&test));
                m_threads[i].startupOk = m_threads[i].thread->start();
            }

            // ...then block until they all terminate.
            // Whilst performing the join, this thread will
            // be implicitly safepointed.
            for (int i = 0; i < test.threadQty(); i++) {
                if (m_doJoin && m_threads[i].startupOk) {
                    m_threads[i].thread->join();
                }
                mmfx_delete(m_threads[i].thread);
            }

            mmfx_delete_array(m_threads);

            // Cleanup
            m_safepointManager.leave(&spRecord);
        }

        SafepointManager& safepointManager()
        {
            return m_safepointManager;
        }

    private:
        ThreadRecord* m_threads;
        bool m_doJoin;
        SafepointManager m_safepointManager;
    };

    /**
     * Threads increment a shared counter.
     * Thread safety is guaranteed by only performing the increment within a safepoint task.
     */
    class SimpleTest : public SafepointTestBase
    {
        class SimpleSafepointTask : public SafepointTask
        {
        public:
            SimpleSafepointTask(int& counter)
                : m_counter(counter)
            {
            }

            void run()
            {
                // We're in the safepoint task, so update
                // the shared counter;
                m_counter++;
            }

        private:
            int& m_counter;
        };

    public:
        SimpleTest(SafepointManager& safepointManager, int iterations, int threadQty)
            : SafepointTestBase(safepointManager, iterations, threadQty)
        {
        }

        virtual ~SimpleTest()
        {
        }

        // Thread entrypoint
        virtual void runInSafepointManagerContext()
        {
            for (int i = 0; i < m_iterations; i++) {
                // Increment shared counter in a safepoint task
                SimpleSafepointTask task(sharedCounter);
                m_safepointManager.requestSafepointTask(task);
            }
        }
    };

    /**
     * Producer-consumer with safepoints.
     *
     * Producer threads each increment a *private* counter in a tight
     * loop which includes a safepoint.
     *
     * A single consumer thread executes a tight loop which continually
     * requests that the producers be safepointed. In the safepoint task
     * the consumer 'steals' and then zeros the private counter of the
     * producers. Each stolen counter value is added to the consumer's
     * private counter.
     *
     * This continues until the producers have executed a fixed number of
     * iterations. At the end of this process the consumer should have
     * a deterministic private counter value.
     *
     */
    class ProducerConsumerTest : public SafepointTestBase
    {
    public:

        class ConsumerSafepointTask : public SafepointTask
        {
        public:
            ConsumerSafepointTask(int* producerCounters, int numProducers)
                : m_producerCounters(producerCounters)
                , m_numProducers(numProducers)
                , m_producerStolenCounter(0)
            {
            }

            void run()
            {
                // We're in the safepoint task, so we can steal and then zero
                // the producers' counters.
                // Note that we don't know how many producer threads are still
                // alive, m_numProducers is just the length of the array
                // we inspect.
                for (int i = 0; i < m_numProducers; i++) {
                    m_producerStolenCounter += m_producerCounters[i];
                    m_producerCounters[i] = 0;
                }
            }

            int producerStolenCounter()
            {
                return m_producerStolenCounter;
            }

        private:
            int* const m_producerCounters;
            const int m_numProducers;
            int m_producerStolenCounter;
        };

    public:
        ProducerConsumerTest(SafepointManager& safepointManager, int iterations, int threadQty)
            : SafepointTestBase(safepointManager, iterations, threadQty)
            , m_producerCounts(mmfx_new_array_opt(int, threadQty - 1, kZero))
        {
        }

        virtual ~ProducerConsumerTest()
        {
        }

        // Thread entrypoint
        virtual void runInSafepointManagerContext()
        {
            // Wait for all the threads to startup to force as much
            // contention as possible.
            const int threadIndex = m_threadIn++;
            SCOPE_LOCK_SP_NAMED(locker, m_monitor) {
                locker.notifyAll();
                while (m_threadIn < m_threadQty) {
                    locker.wait();
                }
            }

            // Setup one of the threads as the consumer, the others are producers
            if (threadIndex == m_threadQty - 1) {
                // The consumer continually makes SafepointTask requests on
                // the SafepointManager, leading to safepointed producers.
                // In each SafepointTask the consumer takes each producer's
                // currently accrued, private counter, and adds it
                // to its own total count, then zeroes the producers counter.
                int consumerCount = 0;
                enum State
                {
                    RUNNING, DRAINING, DONE
                };
                State state = RUNNING;
                do {
                    ConsumerSafepointTask task(m_producerCounts, m_threadQty - 1);
                    m_safepointManager.requestSafepointTask(task);
                    consumerCount += task.producerStolenCounter();
                    if (state == DRAINING) {
                        state = DONE;
                    } else if (m_threadOut == m_threadQty - 1) {
                        state = DRAINING;
                    }
                } while (state != DONE);

                // sharedCounter is verified by the selftest runner
                sharedCounter = consumerCount + m_iterations;

                // Can't do this in the dtor, if the selftest verify fails, it will
                // throw an exception and the dtor will not run.
                mmfx_delete_array(m_producerCounts);
            } else {
                // The producers increment their private counter, but we make sure that they
                // reach an explicit safepoint every iteration.
                for (int i = 0; i < m_iterations; i++) {
                    SAFEPOINT_POLL_FAST(m_safepointManager);
                    m_producerCounts[threadIndex]++;
                    if (i % 100 == 0)
                        VMPI_threadYield(); // Try to allow some interesting interleavings
                }
            }
            m_threadOut++;
        }
    private:
        int* m_producerCounts;
        AtomicCounter32 m_threadIn;
        AtomicCounter32 m_threadOut;
    };

    /**
     * Producer-consumer with nested/recursive (arbitrary depth) SafepointManagers.
     *
     * This test uses the same idea as ProducerConsumerTest, but instead of
     * having only one SafepointManager which is entered by all the producer
     * threads, there are N SafepointManagers which the producers continually
     * nest in and out of. Each producer maintains a private counter for each
     * SafepointManager nesting level, which it only increments when in the
     * context of the corresponding SafepointManager.
     *
     * Similar to the producers, the consumer continually nests in and out of
     * the stack of SafepointManagers. At each nesting level the consumer
     * requests that the producers be safepointed with respect to the
     * SafepointManager at that nesting level. In the safepoint task
     * the consumer 'steals' and then zeros the private counter of the
     * producers for that level. Each stolen counter value is added to
     * the consumer's private counter.
     *
     * This continues until the producers have executed a fixed number of
     * iterations for each nesting level. At the end of this process the
     * consumer should have a deterministic private counter value.
     *
     * Note, to test both recursive and nested SafepointManager entry,
     * the SafepointManager used for the final nesting level is the
     * same as the first.
     */
    class NestedProducerConsumerTest : public SafepointTestBase
    {

    public:
        NestedProducerConsumerTest(SafepointManager& safepointManager, int iterations, int threadQty, int nestingDepth)
            : SafepointTestBase(safepointManager, iterations, threadQty)
            , m_nestingDepth(nestingDepth)
        {
            m_nestedManagers = mmfx_new_array(SafepointManager, m_nestingDepth);
            m_producerCounters = mmfx_new_array(int*, m_nestingDepth + 1);
            for (int i = 0; i < m_nestingDepth + 1; i++) {
                m_producerCounters[i] = mmfx_new_array_opt(int, threadQty - 1, kZero);
            }
        }

        virtual ~NestedProducerConsumerTest()
        {
        }

        int nestedConsume(int depth)
        {
            if (depth == -1) {
                // Fish for asserts:
                // At the deepest nesting level recurse into the oldest SafepointManager on the stack.
                SafepointRecord record;
                m_safepointManager.enter(&record);
                ProducerConsumerTest::ConsumerSafepointTask task(m_producerCounters[0], m_threadQty - 1);
                m_safepointManager.requestSafepointTask(task);
                m_safepointManager.leave(&record);
                return task.producerStolenCounter();;
            }
            SafepointRecord record;
            m_nestedManagers[depth].enter(&record);
            ProducerConsumerTest::ConsumerSafepointTask task(m_producerCounters[depth + 1], m_threadQty - 1);
            m_nestedManagers[depth].requestSafepointTask(task);
            int consumerCount = task.producerStolenCounter();

            // Next SafepointManager
            consumerCount += nestedConsume(depth - 1);

            m_nestedManagers[depth].leave(&record);
            return consumerCount;
        }

        void nestedProduce(int depth, int iterations, int threadIndex)
        {
            if (depth == -1) {
                // Fish for asserts:
                // At the deepest nesting level recurse into the oldest SafepointManager on the stack.
                SafepointRecord record;
                m_safepointManager.enter(&record);
                for (int i = 0; i < iterations; i++) {
                    SAFEPOINT_POLL_FAST(m_safepointManager);
                    m_producerCounters[0][threadIndex]++;
                }
                m_safepointManager.leave(&record);
                return;
            }
            SafepointRecord record;
            m_nestedManagers[depth].enter(&record);
            for (int i = 0; i < iterations; i++) {
                SAFEPOINT_POLL_FAST(m_nestedManagers[depth]);
                m_producerCounters[depth + 1][threadIndex]++;
            }
            VMPI_threadYield(); // Try to allow some interesting interleavings

            // Next SafepointManager
            nestedProduce(depth - 1, iterations, threadIndex);

            m_nestedManagers[depth].leave(&record);
        }

        // Thread entrypoint
        virtual void runInSafepointManagerContext()
        {
            // Wait for all the threads to startup to force as much
            // contention as possible.
            const int threadIndex = m_threadIn++;
            SCOPE_LOCK_SP_NAMED(locker, m_monitor) {
                locker.notifyAll();
                while (m_threadIn < m_threadQty) {
                    locker.wait();
                }
            }

            // Setup one of the threads as the consumer, the others are producers
            if (threadIndex == m_threadQty - 1) {
                // The consumer continually makes SafepointTask requests on
                // the SafepointManager at each nesting level, leading to
                // safepointed producers. In each SafepointTask the consumer
                // takes each producer's currently accrued, private counter at
                // the current nesting depth, and adds it to its own total count,
                // then zeroes the producers counter.
                int consumerCount = 0;
                enum State
                {
                    RUNNING, DRAINING, DONE
                };
                State state = RUNNING;
                do {
                    ProducerConsumerTest::ConsumerSafepointTask task(m_producerCounters[0], m_threadQty - 1);
                    m_safepointManager.requestSafepointTask(task);
                    consumerCount += task.producerStolenCounter();
                    consumerCount += nestedConsume(m_nestingDepth - 1);
                    if (state == DRAINING) {
                        state = DONE;
                    } else if (m_threadOut == m_threadQty - 1) {
                        state = DRAINING;
                    }
                } while (state != DONE);

                // sharedCounter is verified by the selftest runner
                sharedCounter = consumerCount + m_iterations;

                // Can't do this in the dtor, if the selftest verify fails, it will
                // throw an exception and the dtor will not run.
                for (int i = 0; i < m_nestingDepth + 1; i++) {
                    mmfx_delete_array(m_producerCounters[i]);
                }
                mmfx_delete_array(m_producerCounters);
                mmfx_delete_array(m_nestedManagers);
            } else {
                // How many increments should we perform each time we visit
                // a nesting depth?
                const int incPerDepth = m_iterations / 1000;
                const int iterations = m_iterations / (incPerDepth * (m_nestingDepth + 2));
                // The producers increment their private counter at each nesting depth,
                // but we make sure that they reach an explicit safepoint every iteration.
                for (int i = 0; i < iterations; i++) {
                    for (int j = 0; j < incPerDepth; j++) {
                        SAFEPOINT_POLL_FAST(m_safepointManager);
                        m_producerCounters[0][threadIndex]++;
                    }
                    nestedProduce(m_nestingDepth - 1, incPerDepth, threadIndex);
                    VMPI_threadYield(); // Try to allow some interesting interleavings
                }
                const int balance = m_iterations % (incPerDepth * (m_nestingDepth + 2));
                for (int i = 0; i < balance; i++) {
                    SAFEPOINT_POLL_FAST(m_safepointManager);
                    m_producerCounters[0][threadIndex]++;
                }
            }
            m_threadOut++;
        }
    private:
        int** m_producerCounters;
        const int m_nestingDepth;
        SafepointManager* m_nestedManagers;
        AtomicCounter32 m_threadIn;
        AtomicCounter32 m_threadOut;
    };

// Must be >=2 for ProducerConsumerTest and NestedProducerConsumerTest
#define THREAD_QTY 4

// Must be >= 1000 for NestedProducerConsumerTest
#define ITERATIONS 100000

// For NestedProducerConsumerTest
#define NESTING_DEPTH 7

class ST_vmbase_safepoints : public Selftest {
public:
ST_vmbase_safepoints(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
};
ST_vmbase_safepoints::ST_vmbase_safepoints(AvmCore* core)
    : Selftest(core, "vmbase", "safepoints", ST_vmbase_safepoints::ST_names,ST_vmbase_safepoints::ST_explicits)
{}
const char* ST_vmbase_safepoints::ST_names[] = {"simple","producer_consumer","nested_producer_consumer", NULL };
const bool ST_vmbase_safepoints::ST_explicits[] = {false,false,false, false };
void ST_vmbase_safepoints::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
}
}
void ST_vmbase_safepoints::test0() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY!=0);
    SimpleTest test(runner.safepointManager(), ITERATIONS, THREAD_QTY);
    runner.runTest(test);
// line 496 "ST_vmbase_safepoints.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_safepoints::test1() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY!=0);
    ProducerConsumerTest test(runner.safepointManager(), ITERATIONS, THREAD_QTY);
    runner.runTest(test);
// line 504 "ST_vmbase_safepoints.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS, "test.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmbase_safepoints::test2() {
#ifndef UNDER_CE
    TestRunner runner(THREAD_QTY!=0);
    NestedProducerConsumerTest test(runner.safepointManager(), ITERATIONS * 100, THREAD_QTY, NESTING_DEPTH);
    runner.runTest(test);
// line 512 "ST_vmbase_safepoints.st"
verifyPass(test.sharedCounter == THREAD_QTY * ITERATIONS * 100, "test.sharedCounter == THREAD_QTY * ITERATIONS * 100", __FILE__, __LINE__);
#endif




}
void create_vmbase_safepoints(AvmCore* core) { new ST_vmbase_safepoints(core); }
}
}
#endif
#endif

// Generated from ST_vmpi_threads.st
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

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_vmpi_threads {
using namespace MMgc;

    class ThreadTestBase {
        public:
            struct ThreadRecord {
                vmpi_thread_t thread;
                int id;
                bool startupOk;
            };

            ThreadTestBase(int threadQty, int iterations) : m_threadQty(threadQty), m_iterations(iterations), sharedCounter(0) {
                VMPI_recursiveMutexInit(&m_mutex);
                VMPI_condVarInit(&m_condition);
            }
            virtual ~ThreadTestBase() {
                VMPI_recursiveMutexDestroy(&m_mutex);
                VMPI_condVarDestroy(&m_condition);
            }

            void runTest() {

                ThreadRecord* threads = mmfx_new_array(ThreadRecord, m_threadQty);

                // Start up the threads
                for (int i = 0; i < m_threadQty; i++) {
                    threads[i].id = i;
                    threads[i].startupOk = VMPI_threadCreate(&threads[i].thread, NULL, _workerThreadEntry, this);
                }

                // ...then block until they all terminate
                for (int i = 0; i < m_threadQty; i++) {
                    if (threads[i].startupOk) {
                        VMPI_threadJoin(threads[i].thread);
                    }
                }

                mmfx_delete_array(threads);
            }
            
            static vmpi_thread_rtn_t VMPI_THREAD_START_CC detachbleThreadEntry(vmpi_thread_arg_t) {
                VMPI_threadDetach(VMPI_currentThread());
                return NULL;
            }
            
            static vmpi_thread_rtn_t VMPI_THREAD_START_CC joinableThreadEntry(vmpi_thread_arg_t) {
                return NULL;
            }

        protected:
            virtual void workerThreadEntry() {}

        private:
            static vmpi_thread_rtn_t VMPI_THREAD_START_CC _workerThreadEntry(vmpi_thread_arg_t arg) {
                ((ThreadTestBase*)arg)->workerThreadEntry();
                return NULL;
            }

        protected:
            const int m_threadQty;
            vmpi_mutex_t m_mutex;
            vmpi_condvar_t m_condition;
            const int m_iterations;
            
        public:
            int sharedCounter;
    };
    
    class MutexTest : public ThreadTestBase {
        public:
            MutexTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~MutexTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    VMPI_recursiveMutexUnlock(&m_mutex);
                    VMPI_recursiveMutexUnlock(&m_mutex);
                }
            }
    };
    
    
    class ConditionTest : public ThreadTestBase {
        public:
            ConditionTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations), m_finishedQty(0) {}
            virtual ~ConditionTest() {}

            virtual void workerThreadEntry() {
                if (m_threadQty < 2) {
                    return;
                }

                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    
                    // If there's another thread still active then wait.
                    if (m_finishedQty < m_threadQty - 1) {
                        VMPI_condVarSignal(&m_condition);
                        VMPI_condVarWait(&m_condition, &m_mutex);
                    }
                    
                    // This thread has finished, so let's wake everyone else up
                    if (i == m_iterations - 1) {
                        m_finishedQty++;
                        VMPI_condVarBroadcast(&m_condition);
                    }
                    VMPI_recursiveMutexUnlock(&m_mutex);

                }

            }

        private:
            int m_finishedQty;
    };

    class AtomicIncrementTest : public ThreadTestBase {
        public:
            AtomicIncrementTest(int threadQty, int iterations, bool withBarrier) : ThreadTestBase(threadQty, iterations), m_withBarrier(withBarrier) {}
            virtual ~AtomicIncrementTest() {}

            virtual void workerThreadEntry() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicIncAndGet32WithBarrier(&sharedCounter);
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicIncAndGet32(&sharedCounter);
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    class AtomicDecrementTest : public ThreadTestBase {
        public:
            AtomicDecrementTest(int threadQty, int iterations, bool withBarrier) : ThreadTestBase(threadQty, iterations), m_withBarrier(withBarrier) {}
            virtual ~AtomicDecrementTest() {}

            virtual void workerThreadEntry() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicDecAndGet32WithBarrier(&sharedCounter);
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicDecAndGet32(&sharedCounter);
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    class CASTest : public ThreadTestBase {
        public:
            CASTest(int threadQty, int iterations, bool withBarrier) : ThreadTestBase(threadQty, iterations), m_withBarrier(withBarrier) {}
            virtual ~CASTest() {}

            virtual void workerThreadEntry() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!VMPI_compareAndSwap32WithBarrier(current, next, &sharedCounter));
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!VMPI_compareAndSwap32(current, next, &sharedCounter));
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    /**
     * We protect a shared counter with a Dekker-style lock that has been made
     * sequentially consistent with memory barriers.
     *
     * The idea is that if the barriers are correct, then two threads can compete
     * to update the counter n times each, so that the final counter value is 2n. If
     * the final value is not 2n, then the barriers have failed to ensure sequential
     * consistency.
     *
     * FIXME: bug 609943
     * This seems way too complicated. We have to be confident in the algorithm
     * before considering the barrier implementations, and I'm not convinced as yet.
     * Is there something simpler?
     *
     * This is test is not actually run. The verifyPass below just returns true.
     */
    class MemoryBarrierTest : public ThreadTestBase {
        public:
            MemoryBarrierTest(int iterations) : ThreadTestBase(2, iterations), m_thread0(0), m_thread1(0), m_turn(NULL) {}
            virtual ~MemoryBarrierTest() {}

            virtual void workerThreadEntry() {

                volatile int* me;
                volatile int* other;
                volatile int* const counterp = &sharedCounter;

                VMPI_recursiveMutexLock(&m_mutex);
                if (m_turn == NULL) {
                    me = &m_thread0;
                    other = &m_thread1;
                    m_turn = me;
                } else {
                    me = &m_thread1;
                    other = &m_thread0;
                }
                VMPI_recursiveMutexUnlock(&m_mutex);

                for (int i = 0; i < m_iterations; i++) {
                    // Dekker lock
                    *me = 1;
                    VMPI_memoryBarrier();
                    while (*other == 1) {
                        VMPI_memoryBarrier();
                        if (m_turn == other) {
                            VMPI_memoryBarrier();
                            *me = 0;
                            VMPI_memoryBarrier();
                            while (m_turn == other) {
                                VMPI_memoryBarrier();
                            }
                            VMPI_memoryBarrier();
                            *me = 1;
                            VMPI_memoryBarrier();
                        }
                    }
                    VMPI_memoryBarrier();
                    (*counterp)++;
                    VMPI_memoryBarrier();
                    m_turn = other;
                    VMPI_memoryBarrier();
                    *me = 0;
                    VMPI_memoryBarrier();
                }
        }
        private:
            volatile int m_thread0;
            volatile int m_thread1;
            volatile int* volatile m_turn;
    };
    
    class TryLockTest : public ThreadTestBase {
        public:
            TryLockTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~TryLockTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    int counter = 0;
                    while (!VMPI_recursiveMutexTryLock(&m_mutex)) {
                        ++counter;
                        if((counter & 63) == 0) {
                            VMPI_threadSleep(1);
                        } 
                    }
                    sharedCounter++;
                    VMPI_recursiveMutexUnlock(&m_mutex);
                }
            }
    };
    
    class ConditionWithWaitTest : public ThreadTestBase {
        public:
            ConditionWithWaitTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~ConditionWithWaitTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    VMPI_condVarTimedWait(&m_condition, &m_mutex, 1);
                    VMPI_recursiveMutexUnlock(&m_mutex);
                }
            }
    };
    
    class SleepTest : public ThreadTestBase {
        public:
            SleepTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~SleepTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    VMPI_recursiveMutexUnlock(&m_mutex);
                    VMPI_threadSleep(1);
                }
            }
    };

// This needs to be at least 2 for ConditionTest
#define THREAD_QTY 4

#define ITERATIONS 100000

class ST_vmpi_threads : public Selftest {
public:
ST_vmpi_threads(AvmCore* core);
virtual void run(int n);
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();
void test9();
void test10();
void test11();
void test12();
void test13();
};
ST_vmpi_threads::ST_vmpi_threads(AvmCore* core)
    : Selftest(core, "vmpi", "threads", ST_vmpi_threads::ST_names,ST_vmpi_threads::ST_explicits)
{}
const char* ST_vmpi_threads::ST_names[] = {"mutexes","conditions","atomic_increment_without_barrier","atomic_decrement_without_barrier","compare_and_swap_without_barrier","atomic_increment_with_barrier","atomic_decrement_with_barrier","compare_and_swap_with_barrier","memory_barrier","try_lock","condition_with_wait","sleep","detach","thread_attributes", NULL };
const bool ST_vmpi_threads::ST_explicits[] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false, false };
void ST_vmpi_threads::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
case 5: test5(); return;
case 6: test6(); return;
case 7: test7(); return;
case 8: test8(); return;
case 9: test9(); return;
case 10: test10(); return;
case 11: test11(); return;
case 12: test12(); return;
case 13: test13(); return;
}
}
void ST_vmpi_threads::test0() {
#ifndef UNDER_CE
    MutexTest mutexTest(THREAD_QTY, ITERATIONS);
    mutexTest.runTest();
// line 334 "ST_vmpi_threads.st"
verifyPass(mutexTest.sharedCounter == THREAD_QTY * ITERATIONS, "mutexTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test1() {
#ifndef UNDER_CE
    ConditionTest conditionTest(THREAD_QTY, ITERATIONS);
    conditionTest.runTest();
// line 341 "ST_vmpi_threads.st"
verifyPass(conditionTest.sharedCounter == THREAD_QTY * ITERATIONS, "conditionTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test2() {
#ifndef UNDER_CE
    AtomicIncrementTest atomicIncrementTest(THREAD_QTY, ITERATIONS, false);
    atomicIncrementTest.runTest();
// line 348 "ST_vmpi_threads.st"
verifyPass(atomicIncrementTest.sharedCounter == THREAD_QTY * ITERATIONS, "atomicIncrementTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif
    
}
void ST_vmpi_threads::test3() {
#ifndef UNDER_CE
    AtomicDecrementTest atomicDecrementTest(THREAD_QTY, ITERATIONS, false);
    atomicDecrementTest.runTest();
// line 355 "ST_vmpi_threads.st"
verifyPass(atomicDecrementTest.sharedCounter == -(THREAD_QTY * ITERATIONS), "atomicDecrementTest.sharedCounter == -(THREAD_QTY * ITERATIONS)", __FILE__, __LINE__);
#endif
    
}
void ST_vmpi_threads::test4() {
#ifndef UNDER_CE
    CASTest casTest(THREAD_QTY, ITERATIONS, false);
    casTest.runTest();
// line 362 "ST_vmpi_threads.st"
verifyPass(casTest.sharedCounter == THREAD_QTY * ITERATIONS, "casTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test5() {
#ifndef UNDER_CE
    AtomicIncrementTest atomicIncrementTest(THREAD_QTY, ITERATIONS, true);
    atomicIncrementTest.runTest();
// line 369 "ST_vmpi_threads.st"
verifyPass(atomicIncrementTest.sharedCounter == THREAD_QTY * ITERATIONS, "atomicIncrementTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif
    
}
void ST_vmpi_threads::test6() {
#ifndef UNDER_CE
    AtomicDecrementTest atomicDecrementTest(THREAD_QTY, ITERATIONS, true);
    atomicDecrementTest.runTest();
// line 376 "ST_vmpi_threads.st"
verifyPass(atomicDecrementTest.sharedCounter == -(THREAD_QTY * ITERATIONS), "atomicDecrementTest.sharedCounter == -(THREAD_QTY * ITERATIONS)", __FILE__, __LINE__);
#endif
    
}
void ST_vmpi_threads::test7() {
#ifndef UNDER_CE
    CASTest casTest(THREAD_QTY, ITERATIONS, true);
    casTest.runTest();
// line 383 "ST_vmpi_threads.st"
verifyPass(casTest.sharedCounter == THREAD_QTY * ITERATIONS, "casTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test8() {
#ifndef UNDER_CE
    /* This test is failing on Windows and Mac OSX 10.4.
     * For Windows, see bug 609820.
     * For Mac, are the 10.4 APIs not reliable?
     * It could also be the test, or the compiler!
     * FIXME: bug 609943 Selftests to stress memory barriers (fences)
    
    // Note that the memory barrier test is based on a Dekker lock, so we
    // only ever use 2 threads.
    MemoryBarrierTest memoryBarrierTest(ITERATIONS);
    memoryBarrierTest.runTest();
// line 398 "ST_vmpi_threads.st"
verifyPass(memoryBarrierTest.sharedCounter == 2 * ITERATIONS, "memoryBarrierTest.sharedCounter == 2 * ITERATIONS", __FILE__, __LINE__);
    
    */
// line 401 "ST_vmpi_threads.st"
verifyPass(true, "true", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test9() {
#ifndef UNDER_CE
    TryLockTest tryLockTest(THREAD_QTY, ITERATIONS);
    tryLockTest.runTest();
// line 408 "ST_vmpi_threads.st"
verifyPass(tryLockTest.sharedCounter == THREAD_QTY * ITERATIONS, "tryLockTest.sharedCounter == THREAD_QTY * ITERATIONS", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test10() {
#ifndef UNDER_CE
    // Use 2000 iterations with a 1 ms wait
    ConditionWithWaitTest conditionWithWaitTest(THREAD_QTY, 2000);
    conditionWithWaitTest.runTest();
// line 416 "ST_vmpi_threads.st"
verifyPass(conditionWithWaitTest.sharedCounter == THREAD_QTY * 2000, "conditionWithWaitTest.sharedCounter == THREAD_QTY * 2000", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test11() {
#ifndef UNDER_CE
    // Use 2000 iterations with a 1 ms sleep
    SleepTest sleepTest(THREAD_QTY, 2000);
    sleepTest.runTest();
// line 424 "ST_vmpi_threads.st"
verifyPass(sleepTest.sharedCounter == THREAD_QTY * 2000, "sleepTest.sharedCounter == THREAD_QTY * 2000", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test12() {
#ifndef UNDER_CE
    // We can't really test for much here...
    vmpi_thread_t thread;
    VMPI_threadCreate(&thread, NULL, ThreadTestBase::detachbleThreadEntry, NULL);
// line 432 "ST_vmpi_threads.st"
verifyPass(true, "true", __FILE__, __LINE__);
#endif

}
void ST_vmpi_threads::test13() {
#ifndef UNDER_CE
    vmpi_thread_attr_t attr;
// line 438 "ST_vmpi_threads.st"
verifyPass(VMPI_threadAttrInit(&attr) == true, "VMPI_threadAttrInit(&attr) == true", __FILE__, __LINE__);
    
    VMPI_threadAttrSetGuardSize(&attr, VMPI_threadAttrDefaultGuardSize());
    VMPI_threadAttrSetStackSize(&attr, VMPI_threadAttrDefaultStackSize());
    VMPI_threadAttrSetPriorityLow(&attr);
    VMPI_threadAttrSetPriorityNormal(&attr);
    VMPI_threadAttrSetPriorityHigh(&attr);
    
    vmpi_thread_t thread;
    bool started = VMPI_threadCreate(&thread, &attr, ThreadTestBase::joinableThreadEntry, NULL);
// line 448 "ST_vmpi_threads.st"
verifyPass(started == true, "started == true", __FILE__, __LINE__);
    if (started) {
        VMPI_threadJoin(thread);
    }
    
// line 453 "ST_vmpi_threads.st"
verifyPass(VMPI_threadAttrDestroy(&attr) == true, "VMPI_threadAttrDestroy(&attr) == true", __FILE__, __LINE__);
#endif


}
void create_vmpi_threads(AvmCore* core) { new ST_vmpi_threads(core); }
}
}
#endif

// Generated from ST_workers_Buffer.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if !defined DEBUGGER
namespace avmplus {
namespace ST_workers_Buffers {
class ST_workers_Buffers : public Selftest {
public:
ST_workers_Buffers(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
    MMgc::GC *gc;
    NoSyncSingleItemBuffer *singleItemBuffer;
    NoSyncMultiItemBuffer *multiItemBuffer;
    NonBlockingMultiItemBuffer *nonBlockMultiItemBuffer;
    Atom *atom;
 
};
ST_workers_Buffers::ST_workers_Buffers(AvmCore* core)
    : Selftest(core, "workers", "Buffers", ST_workers_Buffers::ST_names,ST_workers_Buffers::ST_explicits)
{}
const char* ST_workers_Buffers::ST_names[] = {"NonBlockMultiItemConstructor","NonBlockMultiItemEmptyBuffer","NonBlockMultiItemFullBuffer","SingleConstructor","SingleEmptyBuffer","SingleFullBuffer","MultiConstructor","MultiEmptyBuffer", NULL };
const bool ST_workers_Buffers::ST_explicits[] = {false,false,false,false,false,false,false,false, false };
void ST_workers_Buffers::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
case 4: test4(); return;
case 5: test5(); return;
case 6: test6(); return;
case 7: test7(); return;
}
}
void ST_workers_Buffers::prologue() {
    MMgc::GCConfig config;
    gc = new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
	//gc=core->GetGC();
    atom=(Atom *)malloc(100);
    singleItemBuffer=new NoSyncSingleItemBuffer(gc);
    multiItemBuffer=new NoSyncMultiItemBuffer();
    nonBlockMultiItemBuffer=new NonBlockingMultiItemBuffer(gc);

}
void ST_workers_Buffers::epilogue() {
    delete singleItemBuffer;
    delete multiItemBuffer;
    delete nonBlockMultiItemBuffer;
	delete gc;
    free(atom);

}
void ST_workers_Buffers::test0() {
// line 37 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer!=NULL, "nonBlockMultiItemBuffer!=NULL", __FILE__, __LINE__);

}
void ST_workers_Buffers::test1() {
// line 40 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->isEmpty(), "nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 41 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isFull(), "!nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);
// line 42 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->get(atom), "!nonBlockMultiItemBuffer->get(atom)", __FILE__, __LINE__);

}
void ST_workers_Buffers::test2() {
    bool result;
    for (int i=0;i<14;i++) {
        result=nonBlockMultiItemBuffer->put(String::createLatin1(core,"test string")->atom());
// line 48 "ST_workers_Buffer.st"
verifyPass(result, "result", __FILE__, __LINE__);
// line 49 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isEmpty(), "!nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 50 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isFull(), "!nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);
    }
    result=nonBlockMultiItemBuffer->put(String::createLatin1(core,"test string")->atom());
// line 53 "ST_workers_Buffer.st"
verifyPass(result, "result", __FILE__, __LINE__);
// line 54 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isEmpty(), "!nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 55 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->isFull(), "nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);

    result=nonBlockMultiItemBuffer->put(String::createLatin1(core,"test string")->atom());
// line 58 "ST_workers_Buffer.st"
verifyPass(!result, "!result", __FILE__, __LINE__);
// line 59 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isEmpty(), "!nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 60 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->isFull(), "nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);

    for (int i=0;i<14;i++) {
// line 63 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->get(atom), "nonBlockMultiItemBuffer->get(atom)", __FILE__, __LINE__);
// line 64 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isEmpty(), "!nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 65 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isFull(), "!nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);
    }
// line 67 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->get(atom), "nonBlockMultiItemBuffer->get(atom)", __FILE__, __LINE__);
// line 68 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->isEmpty(), "nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 69 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isFull(), "!nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);

// line 71 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->get(atom), "!nonBlockMultiItemBuffer->get(atom)", __FILE__, __LINE__);
// line 72 "ST_workers_Buffer.st"
verifyPass(nonBlockMultiItemBuffer->isEmpty(), "nonBlockMultiItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 73 "ST_workers_Buffer.st"
verifyPass(!nonBlockMultiItemBuffer->isFull(), "!nonBlockMultiItemBuffer->isFull()", __FILE__, __LINE__);

}
void ST_workers_Buffers::test3() {
// line 76 "ST_workers_Buffer.st"
verifyPass(singleItemBuffer!=NULL, "singleItemBuffer!=NULL", __FILE__, __LINE__);

}
void ST_workers_Buffers::test4() {
// line 79 "ST_workers_Buffer.st"
verifyPass(singleItemBuffer->isEmpty(), "singleItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 80 "ST_workers_Buffer.st"
verifyPass(!singleItemBuffer->isFull(), "!singleItemBuffer->isFull()", __FILE__, __LINE__);
// line 81 "ST_workers_Buffer.st"
verifyPass(!singleItemBuffer->get(atom), "!singleItemBuffer->get(atom)", __FILE__, __LINE__);

}
void ST_workers_Buffers::test5() {
    bool result;
    result=singleItemBuffer->put(String::createLatin1(core,"test string")->atom());
// line 86 "ST_workers_Buffer.st"
verifyPass(result, "result", __FILE__, __LINE__);
// line 87 "ST_workers_Buffer.st"
verifyPass(!singleItemBuffer->isEmpty(), "!singleItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 88 "ST_workers_Buffer.st"
verifyPass(singleItemBuffer->isFull(), "singleItemBuffer->isFull()", __FILE__, __LINE__);
    result=singleItemBuffer->put(String::createLatin1(core,"test string")->atom());
// line 90 "ST_workers_Buffer.st"
verifyPass(!result, "!result", __FILE__, __LINE__);
// line 91 "ST_workers_Buffer.st"
verifyPass(singleItemBuffer->get(atom), "singleItemBuffer->get(atom)", __FILE__, __LINE__);
// line 92 "ST_workers_Buffer.st"
verifyPass(singleItemBuffer->isEmpty(), "singleItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 93 "ST_workers_Buffer.st"
verifyPass(!singleItemBuffer->isFull(), "!singleItemBuffer->isFull()", __FILE__, __LINE__);

}
void ST_workers_Buffers::test6() {
// line 96 "ST_workers_Buffer.st"
verifyPass(multiItemBuffer!=NULL, "multiItemBuffer!=NULL", __FILE__, __LINE__);

}
void ST_workers_Buffers::test7() {
// line 99 "ST_workers_Buffer.st"
verifyPass(singleItemBuffer->isEmpty(), "singleItemBuffer->isEmpty()", __FILE__, __LINE__);
// line 100 "ST_workers_Buffer.st"
verifyPass(!singleItemBuffer->isFull(), "!singleItemBuffer->isFull()", __FILE__, __LINE__);
// line 101 "ST_workers_Buffer.st"
verifyPass(!singleItemBuffer->get(atom), "!singleItemBuffer->get(atom)", __FILE__, __LINE__);

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


}
void create_workers_Buffers(AvmCore* core) { new ST_workers_Buffers(core); }
}
}
#endif
#endif

// Generated from ST_workers_NoSyncSingleItemBuffer.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
#if !defined DEBUGGER
namespace avmplus {
namespace ST_workers_NoSyncSingleItemBuffer {
class ST_workers_NoSyncSingleItemBuffer : public Selftest {
public:
ST_workers_NoSyncSingleItemBuffer(AvmCore* core);
virtual void run(int n);
virtual void prologue();
virtual void epilogue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
void test1();
void test2();
void test3();
    MMgc::GC *gc;
	NoSyncSingleItemBuffer *buffer;
    Atom *atom;
 
};
ST_workers_NoSyncSingleItemBuffer::ST_workers_NoSyncSingleItemBuffer(AvmCore* core)
    : Selftest(core, "workers", "NoSyncSingleItemBuffer", ST_workers_NoSyncSingleItemBuffer::ST_names,ST_workers_NoSyncSingleItemBuffer::ST_explicits)
{}
const char* ST_workers_NoSyncSingleItemBuffer::ST_names[] = {"constructor","emptyBuffer","fullBuffer","destroy", NULL };
const bool ST_workers_NoSyncSingleItemBuffer::ST_explicits[] = {false,false,false,false, false };
void ST_workers_NoSyncSingleItemBuffer::run(int n) {
switch(n) {
case 0: test0(); return;
case 1: test1(); return;
case 2: test2(); return;
case 3: test3(); return;
}
}
void ST_workers_NoSyncSingleItemBuffer::prologue() {
    gc=core->GetGC();
    atom=(Atom *)malloc(100);
    buffer=new NoSyncSingleItemBuffer(gc);

}
void ST_workers_NoSyncSingleItemBuffer::epilogue() {
	delete buffer;
    free(atom);
	
}

void ST_workers_NoSyncSingleItemBuffer::test0() {
// line 30 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(buffer!=NULL, "buffer!=NULL", __FILE__, __LINE__);
	
}
void ST_workers_NoSyncSingleItemBuffer::test1() {
// line 33 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(buffer->isEmpty(), "buffer->isEmpty()", __FILE__, __LINE__);
// line 34 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(!buffer->isFull(), "!buffer->isFull()", __FILE__, __LINE__);
// line 35 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(!buffer->get(atom), "!buffer->get(atom)", __FILE__, __LINE__);
	
}
void ST_workers_NoSyncSingleItemBuffer::test2() {
    buffer->put(String::createLatin1(core,"test string")->atom());
// line 39 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(!buffer->isEmpty(), "!buffer->isEmpty()", __FILE__, __LINE__);
// line 40 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(buffer->isFull(), "buffer->isFull()", __FILE__, __LINE__);
// line 41 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(buffer->get(atom), "buffer->get(atom)", __FILE__, __LINE__);
// line 42 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(buffer->isEmpty(), "buffer->isEmpty()", __FILE__, __LINE__);
// line 43 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(!buffer->isFull(), "!buffer->isFull()", __FILE__, __LINE__);
	
}
void ST_workers_NoSyncSingleItemBuffer::test3() {
    buffer->unregisterRoot();
// line 47 "ST_workers_NoSyncSingleItemBuffer.st"
verifyPass(true, "true", __FILE__, __LINE__);

	

}
void create_workers_NoSyncSingleItemBuffer(AvmCore* core) { new ST_workers_NoSyncSingleItemBuffer(core); }
}
}
#endif
#endif

// Generated from ST_workers_Promise.st
// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "avmshell.h"
#ifdef VMCFG_SELFTEST
namespace avmplus {
namespace ST_workers_Promise {
class ST_workers_Promise : public Selftest {
public:
ST_workers_Promise(AvmCore* core);
virtual void run(int n);
virtual void prologue();
private:
static const char* ST_names[];
static const bool ST_explicits[];
void test0();
    Isolate *main;

};
ST_workers_Promise::ST_workers_Promise(AvmCore* core)
    : Selftest(core, "workers", "Promise", ST_workers_Promise::ST_names,ST_workers_Promise::ST_explicits)
{}
const char* ST_workers_Promise::ST_names[] = {"MainWorker", NULL };
const bool ST_workers_Promise::ST_explicits[] = {false, false };
void ST_workers_Promise::run(int n) {
switch(n) {
case 0: test0(); return;
}
}
void ST_workers_Promise::prologue() {
    main=core->getIsolate();


}
void ST_workers_Promise::test0() {
    // verify the main worker can be retrieved from AvmCore
// line 19 "ST_workers_Promise.st"
verifyPass(main!=NULL, "main!=NULL", __FILE__, __LINE__);
    uint32_t state=main->getAggregate()->queryState(main);
    // verify state == RUNNING
    //printf("state=%d\n",state);
// line 23 "ST_workers_Promise.st"
verifyPass(state==4, "state==4", __FILE__, __LINE__);

// line 25 "ST_workers_Promise.st"
verifyPass(main->getAvmCore() == core, "main->getAvmCore() == core", __FILE__, __LINE__);
// line 26 "ST_workers_Promise.st"
verifyPass(main->isParentOf(main) == false, "main->isParentOf(main) == false", __FILE__, __LINE__);


}
void create_workers_Promise(AvmCore* core) { new ST_workers_Promise(core); }
}
}
#endif

