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

%%component avmplus
%%category vector_accessors

%%prefix 

int32_t reference_getUintPropertyFast(IntVectorObject* obj, uint32_t x)
{
    return obj->getUintPropertyFast(x);
}

void reference_setUintPropertyFast(IntVectorObject* obj, uint32_t x, int32_t v)
{
    obj->setUintPropertyFast(x, v);
}

%%test getOrSetUintPropertyFast

%%verify true

%%test DataListAccessor_on_int

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

%%verify xs[0] == 1
%%verify xs[1] == 1
%%verify xs[2] == 2
%%verify xs[3] == 3
%%verify xs[4] == 5
%%verify xs[5] == 8
%%verify xs[6] == 13

%%test DataListAccessor_on_float4

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

%%verify f4_eq_i(x4s[0], x0) == 1
%%verify f4_eq_i(x4s[1], x1) == 1
%%verify f4_eq_i(x4s[2], x2) == 1

#else

%%verify true

#endif

%%test VectorAccessor_on_int

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

%%verify xs[0] == 1
%%verify xs[1] == 1
%%verify xs[2] == 2
%%verify xs[3] == 3
%%verify xs[4] == 5
%%verify xs[5] == 8
%%verify xs[6] == 13

#else

%%verify true

#endif // AVMSHELL_BUILD

%%test VectorAccessor_on_float4

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

%%verify f4_eq_i(x4s[0], x0) == 1
%%verify f4_eq_i(x4s[1], x1) == 1
%%verify f4_eq_i(x4s[2], x2) == 1

#else

%%verify true

#endif // AVMSHELL_BUILD
