/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_AOT

#include "AOTStubs.h"
#include <math.h>

#pragma GCC visibility push(hidden)

template <> LLVMAtom abcOP_box<LLVMAtom, LLVMBool>(MethodEnv *, LLVMBool b) {
    return (LLVMAtom)((int32_t)b == 1 ? trueAtom : falseAtom);
}

template <> LLVMAtom abcOP_box<LLVMAtom, ScriptObject*>(MethodEnv *, ScriptObject* o) {
    return (LLVMAtom)o->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, ArrayObject*>(MethodEnv *, ArrayObject* o) {
    return (LLVMAtom)o->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, DoubleVectorObject*>(MethodEnv *, DoubleVectorObject* o) {
    return (LLVMAtom)o->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, IntVectorObject*>(MethodEnv *, IntVectorObject* o) {
    return (LLVMAtom)o->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, UIntVectorObject*>(MethodEnv *, UIntVectorObject* o) {
    return (LLVMAtom)o->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, ObjectVectorObject*>(MethodEnv *, ObjectVectorObject* o) {
    return (LLVMAtom)o->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, String*>(MethodEnv *, String* s) {
    return (LLVMAtom)s->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, int32_t>(MethodEnv *env, int32_t i) {
    return (LLVMAtom)env->core()->intToAtom(i);
}

template <> LLVMAtom abcOP_box<LLVMAtom, uint32_t>(MethodEnv *env, uint32_t i) {
    return (LLVMAtom)env->core()->uintToAtom(i);
}

template <> LLVMAtom abcOP_box<LLVMAtom, double>(MethodEnv *env, double d) {
    return (LLVMAtom)env->core()->doubleToAtom(d);
}

template <> LLVMAtom abcOP_box<LLVMAtom, QNameObject*>(MethodEnv *, QNameObject* q) {
    return (LLVMAtom)q->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, Namespace*>(MethodEnv *, Namespace* n) {
    return (LLVMAtom)n->atom();
}

template <> LLVMAtom abcOP_box<LLVMAtom, LLVMUnusedParam>(MethodEnv *, LLVMUnusedParam) {
    AvmAssert(false);
    return (LLVMAtom)undefinedAtom;
}

//------------------------------------------------------------------------------

void throwNullOrUndefined(MethodEnv* env, Atom atom) /* __attribute__((noreturn)) */
{
    AvmAssert(AvmCore::isNullOrUndefined(atom));
    // This could be made tighter by calling nullcheckfail(), but that's private and not really worth making public.
    env->nullcheck(atom);
    // Appease g++, avoid 'noreturn' function does return, profit
    while(1) {}
}

void throwNull(MethodEnv* env) /* __attribute__((noreturn)) */
{
    ScriptObject* o = NULL;
    // This could be made tighter by calling nullcheckfail(), but that's private and not really worth making public.
    env->nullcheck(o->atom());
    // Appease g++, avoid 'noreturn' function does return, profit
    while(1) {}
}


//------------------------------------------------------------------------------

template <> void abcOP_box(MethodEnv *, LLVMAtom)                   {}
template <> LLVMAtom abcOP_box(MethodEnv *, LLVMAtom x)             {return x;}
template <> LLVMBool abcOP_box(MethodEnv *, LLVMBool x)             {return x;}
template <> ScriptObject* abcOP_box(MethodEnv *, ScriptObject* x)   {return x;}
template <> ArrayObject* abcOP_box(MethodEnv *, ArrayObject* x)     {return x;}
template <> ScriptObject* abcOP_box(MethodEnv *, ArrayObject* x)    {return x;}
template <> ArrayObject* abcOP_box(MethodEnv *, ScriptObject* x)    {return (ArrayObject*)x;}
//template <> String* abcOP_box(MethodEnv *env, ScriptObject* x)    { return (String*)x; }
template <> String* abcOP_box(MethodEnv *, String* x)               {return x;}
template <> int32_t abcOP_box(MethodEnv *, int32_t x)               {return x;}
template <> int32_t abcOP_box(MethodEnv *, LLVMBool x)              {return (int32_t)((x == 0) ? 0 : 1);}
template <> uint32_t abcOP_box(MethodEnv *, uint32_t x)             {return x;}
template <> double abcOP_box(MethodEnv *, double x)                 {return x;}
template <> QNameObject* abcOP_box(MethodEnv *, QNameObject* x)     {return x;}
template <> Namespace* abcOP_box(MethodEnv *, Namespace* x)         {return x;}
template <> ScriptObject *abcOP_box(MethodEnv *, String*)           {AvmAssert(false); return NULL;}
template <> double abcOP_box(MethodEnv*, int32_t x)                 {return x;}

// All non-templated stub functions should be defined in this file and not AOTStubs.h.  This will prevent
// non-templated stubs from being multiply defined.

//------------------------------------------------------------------------------
// Debugging helper functions
//------------------------------------------------------------------------------

void abcOP_printf(/*const*/char *s) { // TODO no good way of capturing const-ness of this param as LLVM type for mangling on LLVMEmitter side
    VMPI_log(s);
}

void abcOP_abort(char *s)
{
    VMPI_log(s);
    VMPI_debugBreak();
}

//------------------------------------------------------------------------------
// Calling convention helper functions
//------------------------------------------------------------------------------

LLVMAtom abcOP_argv(MethodEnv*, ArrayObject *a) // TODO LLVMAtom return type is gross
{
    return (LLVMAtom)a->getDenseCopy();
}

int32_t abcOP_argc(MethodEnv*, ArrayObject *a)
{
    return a->getDenseLength();
}

//------------------------------------------------------------------------------
// MethodFrame: DXNS and CodeContext support
//------------------------------------------------------------------------------

void abcOP_enterMethodFrame(MethodEnv* env, AOTMethodFrame* methodFrame) {
    methodFrame->enter(env->core(), env);
}

void abcOP_exitMethodFrame(MethodEnv* env, AOTMethodFrame* methodFrame) {
    methodFrame->exit(env->core());
}

#ifdef VMCFG_TELEMETRY_SAMPLER
void abcOP_enterMethodFrameWithSampler(MethodEnv* env, AOTMethodFrame* methodFrame) {
    // check if we should take a sample
    if (env->core()->sampleTicks)
        env->core()->takeSample();

    methodFrame->enter(env->core(), env);
}

void abcOP_exitMethodFrameWithSampler(MethodEnv* env, AOTMethodFrame* methodFrame) {
    // check if we should take a sample
    if (env->core()->sampleTicks)
        env->core()->takeSample();

    methodFrame->exit(env->core());
}
#endif

void abcOP_dxns(MethodEnv* env, AOTMethodFrame *methodFrame, uint32_t index) {
    Namespace* dxns = env->core()->newPublicNamespace(env->method->pool()->getString(index));
    methodFrame->set_dxns(dxns);
}

//------------------------------------------------------------------------------
// OP_throw / exception stuff
//------------------------------------------------------------------------------

void abcOP_beginTry(MethodEnv* env, ExceptionFrame* ef)
{
#if STUBS_VERBOSE
    env->core()->console << "beginTry: " << hexAddr((uintptr_t)ef) << " : " << hexAddr((uintptr_t)env->core()->exceptionFrame) << "\n";
#endif
    ef->beginLlvmUnwindTry(env->core());
}

void abcOP_endTry(MethodEnv* env, ExceptionFrame* ef) __attribute__((noinline));

void abcOP_endTry(MethodEnv* env, ExceptionFrame* ef)
{
    (void)env;
#if STUBS_VERBOSE
    env->core()->console << "endTry: " << hexAddr((uintptr_t)ef) << " : " << hexAddr((uintptr_t)env->core()->exceptionFrame) << "\n";
#endif
    ef->endTry();
}

void abcOP_beginCatch(MethodEnv* env, ExceptionFrame* ef) __attribute__((noinline));

void abcOP_beginCatch(MethodEnv* env, ExceptionFrame* ef)
{
    (void)env;
#if STUBS_VERBOSE
    env->core()->console << "beginCatch: " << hexAddr((uintptr_t)ef) << " : " << hexAddr((uintptr_t)env->core()->exceptionFrame) << "\n";
#endif
    ef->beginCatch();
}

void abcOP_rethrow(MethodEnv* env, ExceptionFrame* ef) __attribute__((noinline));

void abcOP_rethrow(MethodEnv* env, ExceptionFrame* ef)
{
    (void)ef;
#if STUBS_VERBOSE
    env->core()->console << "rethrow: " << hexAddr((uintptr_t)ef) << " : " << hexAddr((uintptr_t)env->core()->exceptionFrame) << "\n";
#endif
    AvmCore* core = env->core();
    core->throwAtom(core->exceptionAddr->atom);
}

//------------------------------------------------------------------------------
// OP_debug
//------------------------------------------------------------------------------
void abcOP_debugEnter(MethodEnv* env, CallStackNode* csn, LLVMAtom *vars, int32_t* exceptFilt) {
    (void)env;
    (void)csn;
    (void)vars;
    (void)exceptFilt;
#ifdef DEBUGGER
    // We currently mimic the interpreter not the JIT by storing all debug vars
    // in an Atom array as we don't know where (or if) the real value will be
    // on the native stack. The interpreter codepath assumes the values will be
    // Atoms so it doesn't need the SST array the JIT uses, that's why we pas
    // NULL as the first argument to debugEnter here.
    if(env->core()->debugger() != NULL)
        env->debugEnter(NULL, csn, (FramePtr)vars, (volatile intptr_t*)exceptFilt);
#endif
}

void abcOP_debugExit(MethodEnv* env, CallStackNode* csn) {
    (void)env;
    (void)csn;
#ifdef DEBUGGER
    if(env->core()->debugger() != NULL)
        env->debugExit(csn);
#endif
}

void abcOP_debug(MethodEnv* env, uint32_t i, uint32_t nameId, uint32_t slotNum, uint32_t lineNo) {
    (void)env;
    (void)i;
    (void)nameId;
    (void)slotNum;
    (void)lineNo;
    if(i == 1 /* DI_LOCAL */)
    {
        Stringp name = env->method->pool()->getString(nameId);

        (void)name;
#ifdef DEBUGGER
        if(env->core()->debugger() != NULL)
            env->method->setRegName(slotNum, name);
#endif
#if STUBS_VERBOSE
        env->core()->console << "op_debug: " << name << " : slot # " << slotNum << " / line # " << lineNo << "\n";
#endif
    }
}

//------------------------------------------------------------------------------
// OP_debugfile
//------------------------------------------------------------------------------
void abcOP_debugfile(MethodEnv* env, uint32_t index) {
    (void)env;
    (void)index;
#ifdef DEBUGGER
    AvmCore *core = env->core();
    avmplus::Debugger *debugger = core->debugger();
    if(debugger != NULL)
        debugger->debugFile( env->method->pool()->getString(index));
#endif
}

//------------------------------------------------------------------------------
// OP_debugline
//------------------------------------------------------------------------------
void abcOP_debugline(MethodEnv* env, uint32_t line) {
    (void)env;
    (void)line;
#ifdef DEBUGGER
    AvmCore *core = env->core();
    avmplus::Debugger *debugger = core->debugger();
    if(debugger != NULL)
        debugger->debugLine(line);
#endif
}

//------------------------------------------------------------------------------
// type_known_to_be_compatible
//------------------------------------------------------------------------------

template <> bool type_known_to_be_compatible<LLVMAtom>(MethodEnv *, Traits *t)
{
    return Traits::isSSTAtom(t);
}

template <> bool type_known_to_be_compatible<int32_t>(MethodEnv *env, Traits *t)
{
    AvmCore *core = env->core();
    return t == INT_TYPE || t == UINT_TYPE;
}

template <> bool type_known_to_be_compatible<uint32_t>(MethodEnv *env, Traits *t)
{
    AvmCore *core = env->core();
    return t == INT_TYPE || t == UINT_TYPE;
}

template <> bool type_known_to_be_compatible<double>(MethodEnv *env, Traits *t)
{
    AvmCore *core = env->core();
    return t == NUMBER_TYPE;
}

template <> bool type_known_to_be_compatible<LLVMBool>(MethodEnv *env, Traits *t)
{
    AvmCore *core = env->core();
    return t == BOOLEAN_TYPE;
}

template <> bool type_known_to_be_compatible<Stringp>(MethodEnv *env, Traits *t)
{
    AvmCore *core = env->core();
    return t == STRING_TYPE;
}

template <> bool type_known_to_be_compatible<void>(MethodEnv *env, Traits *t)
{
    AvmCore *core = env->core();
    return t == VOID_TYPE;
}

template <> bool type_known_to_be_compatible<ScriptObject *>(MethodEnv*, Traits *t)
{
    if (t->isResolved())
        return t->isSSTObject();
    // if the traits are not yet linked, then we don't know if there are compatible.
    return false;
}

//------------------------------------------------------------------------------
// Un-Boxing
//------------------------------------------------------------------------------


template <>
ScriptObject* abcOP_unbox<ScriptObject*, Atom>(MethodEnv *, Atom a) {
    return AvmCore::atomToScriptObject(a);
}

template <>
ArrayObject* abcOP_unbox<ArrayObject*, Atom>(MethodEnv *, Atom a) {
    return (ArrayObject *)AvmCore::atomToScriptObject(a);
}

template <>
DoubleVectorObject* abcOP_unbox<DoubleVectorObject*, Atom>(MethodEnv *, Atom a) {
    return (DoubleVectorObject *)AvmCore::atomToScriptObject(a);
}

template <>
IntVectorObject* abcOP_unbox<IntVectorObject*, Atom>(MethodEnv *, Atom a) {
    return (IntVectorObject *)AvmCore::atomToScriptObject(a);
}

template <>
UIntVectorObject* abcOP_unbox<UIntVectorObject*, Atom>(MethodEnv *, Atom a) {
    return (UIntVectorObject *)AvmCore::atomToScriptObject(a);
}

template <>
ObjectVectorObject* abcOP_unbox<ObjectVectorObject*, Atom>(MethodEnv *, Atom a) {
    return (ObjectVectorObject *)AvmCore::atomToScriptObject(a);
}

template <>
String* abcOP_unbox<String*, Atom>(MethodEnv *, Atom a) {
    return AvmCore::atomToString(a);
}

template <>
String* abcOP_unbox<String*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::atomToString((Atom)a);
}

template <>
int32_t abcOP_unbox<int32_t, Atom>(MethodEnv *, Atom a) {
    return AvmCore::integer_i(a);
}

template <>
LLVMAtom abcOP_unbox<LLVMAtom, Atom>(MethodEnv *, Atom a) {
    return (LLVMAtom)a;
}

template <>
LLVMBool abcOP_unbox<LLVMBool, Atom>(MethodEnv *, Atom a) {
    return (LLVMBool)(a == trueAtom ? 1 : 0);
}

template <>
Namespace* abcOP_unbox<Namespace*, Atom>(MethodEnv *, Atom a) {
    return AvmCore::atomToNamespace(a);
}

template <>
uint32_t abcOP_unbox<uint32_t, Atom>(MethodEnv *, Atom a) {
    return AvmCore::integer_u(a);
}

template <>
QNameObject* abcOP_unbox<QNameObject*, Atom>(MethodEnv *, Atom a) {
    return AvmCore::atomToQName(a);
}

template <>
double abcOP_unbox<double, Atom>(MethodEnv *, Atom a) {
    return AvmCore::number(a);
}

template <>
void abcOP_unbox<void, Atom>(MethodEnv *, Atom) {
}

//------------------------------------------------------------------------------

template <>
ScriptObject* abcOP_unbox<ScriptObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::atomToScriptObject((Atom)a);
}

template <>
ArrayObject* abcOP_unbox<ArrayObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return static_cast<ArrayObject*>(AvmCore::atomToScriptObject((Atom)a));
}

template <>
DoubleVectorObject* abcOP_unbox<DoubleVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return static_cast<DoubleVectorObject*>(AvmCore::atomToScriptObject((Atom)a));
}

template <>
IntVectorObject* abcOP_unbox<IntVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return static_cast<IntVectorObject*>(AvmCore::atomToScriptObject((Atom)a));
}

template <>
UIntVectorObject* abcOP_unbox<UIntVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return static_cast<UIntVectorObject*>(AvmCore::atomToScriptObject((Atom)a));
}

template <>
ObjectVectorObject* abcOP_unbox<ObjectVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return static_cast<ObjectVectorObject*>(AvmCore::atomToScriptObject((Atom)a));
}

template <>
int32_t abcOP_unbox<int32_t, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::integer_i((Atom)a);
}

template <>
LLVMAtom abcOP_unbox<LLVMAtom, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return (LLVMAtom)a;
}

template <>
LLVMBool abcOP_unbox<LLVMBool, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return (LLVMBool)(((Atom)a == trueAtom) ? 1 : 0);
}

template <>
Namespace* abcOP_unbox<Namespace*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::atomToNamespace((Atom)a);
}

template <>
uint32_t abcOP_unbox<uint32_t, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::integer_u((Atom)a);
}

template <>
QNameObject* abcOP_unbox<QNameObject*, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::atomToQName((Atom)a);
}

template <>
double abcOP_unbox<double, LLVMAtom>(MethodEnv *, LLVMAtom a) {
    return AvmCore::number((Atom)a);
}

//------------------------------------------------------------------------------
// OP_coerce
//------------------------------------------------------------------------------

template <>
String* abcOP_coerce<String*, LLVMAtom>(MethodEnv *env, Traits**, LLVMAtom val) {
    return abcOP_coerce_s<String*>(env, val);
}

template <>
LLVMBool abcOP_coerce<LLVMBool, LLVMAtom>(MethodEnv *env, Traits**, LLVMAtom val) {
    return abcOP_unbox<LLVMBool>(env, env->core()->booleanAtom((Atom)val));
}

template <>
int32_t abcOP_coerce<int32_t, LLVMAtom>(MethodEnv *env, Traits**, LLVMAtom val) {
    return abcOP_unbox<int32_t>(env, env->core()->intAtom((Atom)val));
}

template <>
uint32_t abcOP_coerce<uint32_t, LLVMAtom>(MethodEnv *env, Traits**, LLVMAtom val) {
    return abcOP_unbox<uint32_t>(env, env->core()->uintAtom((Atom)val));
}

template <>
double abcOP_coerce<double, LLVMAtom>(MethodEnv *env, Traits**, LLVMAtom val) {
    return abcOP_unbox<double>(env, env->core()->numberAtom((Atom)val));
}

//------------------------------------------------------------------------------
// OP_upcast
//------------------------------------------------------------------------------

template <>
String* abcOP_upcast(MethodEnv*, ScriptObject* val) {
    (void)val;
    AvmAssert(val == 0);
    return NULL;
}

template <>
Namespace* abcOP_upcast(MethodEnv*, ScriptObject* val) {
    (void)val;
    AvmAssert(val == 0);
    return NULL;
}

//------------------------------------------------------------------------------
// OP_convert_b
//------------------------------------------------------------------------------

template <>
LLVMBool abcOP_convert_b(MethodEnv*, double d) {
    return (LLVMBool)(MathUtils::doubleToBool(d) ? 1 : 0);
}

template <>
LLVMBool abcOP_convert_b(MethodEnv*, uint32_t u) {
    return (LLVMBool)(u ? 1 : 0);
}

template <>
LLVMBool abcOP_convert_b(MethodEnv*, int32_t i) {
    return (LLVMBool)(i ? 1 : 0);
}

//------------------------------------------------------------------------------
// OP_not
//------------------------------------------------------------------------------

template <>
LLVMBool abcOP_not(MethodEnv*, LLVMBool a) {
    return (LLVMBool)((a == 0) ? 1 : 0);
}

template <>
LLVMBool abcOP_not(MethodEnv*, double d) {
    return (LLVMBool)(MathUtils::doubleToBool(d) ? 0 : 1);
}

template <>
LLVMBool abcOP_not(MethodEnv*, uint32_t u) {
    return (LLVMBool)(u ? 0 : 1);
}

template <>
LLVMBool abcOP_not(MethodEnv*, int32_t i) {
    return (LLVMBool)(i ? 0 : 1);
}

//------------------------------------------------------------------------------
// OP_bitand
//------------------------------------------------------------------------------

template <> int32_t abcOP_bitand(MethodEnv *, int32_t a, int32_t b) {return (a & b);}

//------------------------------------------------------------------------------
// OP_bitor
//------------------------------------------------------------------------------

template <> int32_t abcOP_bitor(MethodEnv *, int32_t a, int32_t b) {return (a | b);}

//------------------------------------------------------------------------------
// OP_bitxor
//------------------------------------------------------------------------------

template <> int32_t abcOP_bitxor(MethodEnv *, int32_t a, int32_t b) {return (a ^ b);}

//------------------------------------------------------------------------------
// OP_bitnot
//------------------------------------------------------------------------------

template <> int32_t abcOP_bitnot(MethodEnv *, int32_t a) {return (~ a);}

//------------------------------------------------------------------------------
// OP_lshift
//------------------------------------------------------------------------------

template <> int32_t abcOP_lshift(MethodEnv *, int32_t a, int32_t b) {return (int32_t)(a << ((uint32_t)b & 0x1F));}
template <> int32_t abcOP_lshift(MethodEnv *, double a, int32_t b) {return (int32_t)(AvmCore::integer_d(a) << ((uint32_t)b & 0x1F));}

//------------------------------------------------------------------------------
// OP_rshift
//------------------------------------------------------------------------------

template <> int32_t abcOP_rshift(MethodEnv *, int32_t a, int32_t b) {return (int32_t)(a >> ((uint32_t)b & 0x1F));}
template <> int32_t abcOP_rshift(MethodEnv *, double a, int32_t b) {return (int32_t)(AvmCore::integer_d(a) >> ((uint32_t)b & 0x1F));}
template <> int32_t abcOP_rshift(MethodEnv *, uint32_t a, int32_t b) {return (int32_t)((int32_t)a >> ((uint32_t)b & 0x1F));}

//------------------------------------------------------------------------------
// OP_urshift
//------------------------------------------------------------------------------

template <> uint32_t abcOP_urshift(MethodEnv *, uint32_t a, uint32_t b) {return (uint32_t)(a >> (b & 0x1F));}

//------------------------------------------------------------------------------
// OP_negate
//------------------------------------------------------------------------------

template <> int32_t abcOP_negate<int32_t, int32_t> (MethodEnv *, int32_t a) {return (- a);}
template <> int32_t abcOP_negate<int32_t, double> (MethodEnv *, double a) {return (- AvmCore::integer_d(a));}
template <> double abcOP_negate<double, double> (MethodEnv *, double a) {return (- a);}

//------------------------------------------------------------------------------
// OP_negate_i
//------------------------------------------------------------------------------

template <> int32_t abcOP_negate_i<int32_t, int32_t> (MethodEnv *, int32_t a) {return (- a);}
template <> int32_t abcOP_negate_i<int32_t, double> (MethodEnv *, double a) {return (int32_t)(- AvmCore::integer_d(a));}

//------------------------------------------------------------------------------
// OP_loadnan
//------------------------------------------------------------------------------

template <>
double abcOP_loadnan(MethodEnv* env) {
    return abcOP_unbox<double, Atom>(env, env->core()->kNaN);
}

//------------------------------------------------------------------------------
// OP_constructprop
//------------------------------------------------------------------------------

ScriptObject *abcOP_newInstance(MethodEnv *env, LLVMAtom classClosure)
{
	// TODO we're guaranteed that this is a ClassClosure because of the way GO/LLVMEmitter are
	// structured, but it would still be nice to figure out how to assert that...
	nullcheck(env, classClosure);
	return ((ClassClosure *)AvmCore::atomToScriptObject((Atom)classClosure))->newInstance();
}

//------------------------------------------------------------------------------
// OP_callstatic
//------------------------------------------------------------------------------

MethodEnv *abcOP_methodEnvFromIndex(AbcEnv *abcenv, int32_t methodindex)
{
    AvmAssert(abcenv != NULL);
    MethodEnv* res = abcenv->getMethod(methodindex);
    AvmAssert(res != 0);
    return res;
}

MethodEnv *abcOP_methodEnvFromInterface(MethodEnv *env, int32_t functionIID, ScriptObject *so)
{
  AvmAssert(so != NULL);
  VTable *vtable = abcOP_toVTable<true>(env, so);

  InterfaceBindingFunction bindingFunction = (InterfaceBindingFunction)vtable->interfaceBindingFunction;
  AvmAssert(bindingFunction != NULL);

  int methodindex = (*bindingFunction)(functionIID);
  if( methodindex < 1 )
      env->toplevel()->throwTypeErrorWithName(kCallOfNonFunctionError, "value");
    
  MethodEnv *method = vtable->methods[methodindex-1];
  AvmAssert(method != NULL);
  return method;
}

//------------------------------------------------------------------------------
// OP_constructsuper
//------------------------------------------------------------------------------

void abcOP_constructsuper(MethodEnv* env, LLVMAtom *args, int32_t argc) {
    AvmAssert(argc > 0);
    // argc - 1, because "this" does not count.
    env->super_init()->coerceEnter(argc - 1, (Atom*)args);
}

//------------------------------------------------------------------------------
// "OP_Math_abs"
//------------------------------------------------------------------------------

template <> double abcOP_Math_abs<double,int> (MethodEnv *, int32_t a) {return MathUtils::abs( (double)a );}
template <> double abcOP_Math_abs<double,double> (MethodEnv *, double a) {return MathUtils::abs( a );}

//------------------------------------------------------------------------------
// "OP_Math_acos"
//------------------------------------------------------------------------------

template <> double abcOP_Math_acos<double,int> (MethodEnv *, int32_t a) {return MathUtils::acos( (double)a );}
template <> double abcOP_Math_acos<double,double> (MethodEnv *, double a) {return MathUtils::acos( a );}

//------------------------------------------------------------------------------
// "OP_Math_asin"
//------------------------------------------------------------------------------

template <> double abcOP_Math_asin<double,int> (MethodEnv *, int32_t a) {return MathUtils::asin( (double)a );}
template <> double abcOP_Math_asin<double,double> (MethodEnv *, double a) {return MathUtils::asin( a );}

//------------------------------------------------------------------------------
// "OP_Math_atan"
//------------------------------------------------------------------------------

template <> double abcOP_Math_atan<double,int> (MethodEnv *, int32_t a) {return MathUtils::atan( (double)a );}
template <> double abcOP_Math_atan<double,double> (MethodEnv *, double a) {return MathUtils::atan( a );}

//------------------------------------------------------------------------------
// "OP_Math_atan2"
//------------------------------------------------------------------------------

template <> double abcOP_Math_atan2<double, int, int> (MethodEnv *, int32_t a, int32_t b) {return MathUtils::atan2( (double)a, (double)b );}
template <> double abcOP_Math_atan2<double, double, double> (MethodEnv *, double a, double b) {return MathUtils::atan2( a, b );}
template <> double abcOP_Math_atan2<double, int, double> (MethodEnv *, int32_t a, double b) {return MathUtils::atan2( a, (double)b );}
template <> double abcOP_Math_atan2<double, double, int> (MethodEnv *, double a, int32_t b) {return MathUtils::atan2( (double)a, b );}

//------------------------------------------------------------------------------
// "OP_Math_ceil"
//------------------------------------------------------------------------------

template <> double abcOP_Math_ceil<double,int> (MethodEnv *, int32_t a) {return MathUtils::ceil( (double)a );}
template <> double abcOP_Math_ceil<double,double> (MethodEnv *, double a) {return MathUtils::ceil( a );}

//------------------------------------------------------------------------------
// "OP_Math_cos"
//------------------------------------------------------------------------------

template <> double abcOP_Math_cos<double,int> (MethodEnv *, int32_t a) {return MathUtils::cos( (double)a );}
template <> double abcOP_Math_cos<double,double> (MethodEnv *, double a) {return MathUtils::cos( a );}

//------------------------------------------------------------------------------
// "OP_Math_exp"
//------------------------------------------------------------------------------

template <> double abcOP_Math_exp<double,int> (MethodEnv *, int32_t a) {return MathUtils::exp( (double)a );}
template <> double abcOP_Math_exp<double,double> (MethodEnv *, double a) {return MathUtils::exp( a );}

//------------------------------------------------------------------------------
// "OP_Math_floor"
//------------------------------------------------------------------------------

template <> double abcOP_Math_floor<double,int> (MethodEnv *, int32_t a) {return MathUtils::floor( (double)a );}
template <> double abcOP_Math_floor<double,double> (MethodEnv *, double a) {return MathUtils::floor( a );}

//------------------------------------------------------------------------------
// "OP_Math_log"
//------------------------------------------------------------------------------

template <> double abcOP_Math_log<double,int> (MethodEnv *, int32_t a) {return MathUtils::log( (double)a );}
template <> double abcOP_Math_log<double,double> (MethodEnv *, double a) {return MathUtils::log( a );}

//------------------------------------------------------------------------------
// "OP_Math_pow"
//------------------------------------------------------------------------------

template <> double abcOP_Math_pow<double, int, int> (MethodEnv *, int32_t a, int32_t b) {return MathUtils::pow( (double)a, (double)b );}
template <> double abcOP_Math_pow<double, double, double> (MethodEnv *, double a, double b) {return MathUtils::pow( a, b );}
template <> double abcOP_Math_pow<double, int, double> (MethodEnv *, int32_t a, double b) {return MathUtils::pow( a, (double)b );}
template <> double abcOP_Math_pow<double, double, int> (MethodEnv *, double a, int32_t b) {return MathUtils::pow( (double)a, b );}

//------------------------------------------------------------------------------
// "OP_Math_round"
//------------------------------------------------------------------------------

template <> double abcOP_Math_round<double,int> (MethodEnv *, int32_t a) {return MathUtils::round( (double)a );}
template <> double abcOP_Math_round<double,double> (MethodEnv *, double a) {return MathUtils::round( a );}

//------------------------------------------------------------------------------
// "OP_Math_sin"
//------------------------------------------------------------------------------

template <> double abcOP_Math_sin<double,int> (MethodEnv *, int32_t a) {return MathUtils::sin( (double)a );}
template <> double abcOP_Math_sin<double,double> (MethodEnv *, double a) {return MathUtils::sin( a );}

//------------------------------------------------------------------------------
// "OP_Math_sqrt"
//------------------------------------------------------------------------------

template <> double abcOP_Math_sqrt<double,int> (MethodEnv *, int32_t a) {return MathUtils::sqrt( (double)a );}
template <> double abcOP_Math_sqrt<double,double> (MethodEnv *, double a) {return MathUtils::sqrt( a );}

//------------------------------------------------------------------------------
// "OP_Math_tan"
//------------------------------------------------------------------------------

template <> double abcOP_Math_tan<double,int> (MethodEnv *, int32_t a) {return MathUtils::tan( (double)a );}
template <> double abcOP_Math_tan<double,double> (MethodEnv *, double a) {return MathUtils::tan( a );}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

bool doubleIsUint32(double n, uint32_t *out)
{
    uint32_t trunced = (uint32_t)n;
    if(trunced == n)
    {
        *out = trunced;
        return true;
    }
    return false;
}

bool doubleIsInt32(double n, int32_t *out)
{
    int32_t trunced = (int32_t)n;
    if(trunced == n)
    {
        *out = trunced;
        return true;
    }
    return false;
}

bool atomIsUint32(Atom a, uint32_t *out)
{
    switch(atomKind(a))
    {
    case kIntptrType:
        if(atomGetIntptr(a) >= 0)
        {
            *out = atomGetIntptr(a);
            return true;
        }
        break;
    case kDoubleType:
        return doubleIsUint32(AvmCore::atomToDouble(a), out);
    }
    return false;
}

bool atomIsInt32(Atom a, int32_t *out)
{
    switch(atomKind(a))
    {
    case kIntptrType:
        *out = atomGetIntptr(a);
        return true;
    case kDoubleType:
        return doubleIsInt32(AvmCore::atomToDouble(a), out);
    }
    return false;
}


//------------------------------------------------------------------------------
// Multiname helper functions
//------------------------------------------------------------------------------

void applyQNameToMultiName(Multiname &mn, QNameObject* qn) {
    bool attr = mn.isAttr();
    qn->getMultiname(mn);
    // Mindlessly copying from initMultiname in Interpretter.cpp
    if (attr)
        mn.setAttr(attr);
}

#ifdef VMCFG_BUFFER_GUARD
// Lazy eval stuff
template <typename rt> rt abcOP_load(int *addr);

template <> String *abcOP_load(int *addr)
{
    String *result = (String *)LoadRedirSafeLoad((uintptr_t *)addr);
    return result;
}

template <> ScriptObject *abcOP_load(int *addr)
{
    ScriptObject *result = (ScriptObject *)LoadRedirSafeLoad((uintptr_t *)addr);
    return result;
}

template <> MethodEnv *abcOP_load(int *addr)
{
    MethodEnv *result = (MethodEnv *)LoadRedirSafeLoad((uintptr_t *)addr);
    return result;
}

extern "C" {
    uintptr_t lazyEvalPushstring(avmplus::MethodEnv *env, uintptr_t index)
    {
        String *result = (env->method->pool()->getString(index));
#if 0
        StUTF8String s(result);
        printf("pushstring '%s'\n", s.c_str());
#endif
        return (uintptr_t)result;
    }

    uintptr_t lazyEvalFinddef(avmplus::MethodEnv *env, uintptr_t index)
    {
        Multiname defName;
        env->method->pool()->parseMultiname(defName, index);
        AvmAssert(!defName.isRuntime());
#if 0
        String *f = defName.format(env->core());
        StUTF8String s(f);
        const char *sz = s.c_str();
        printf("finddef '%s'\n", sz);
#endif
        return (uintptr_t)env->finddef(&defName);
    }

    uintptr_t lazyEvalMethodEnvFromIndex(avmplus::MethodEnv *env, uintptr_t index)
    {
        MethodEnv* res = env->abcEnv()->getMethod(index);
        AvmAssert(res != 0);
        return (uintptr_t)res;
    }
}
#endif

#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
LLVMAtom abcOP_promoteOptimisticAtom(MethodEnv *env, LLVMAtom a)
{
    return (LLVMAtom)AvmCore::promoteOptimisticAtom(env->core()->gc, (Atom)a);
}
#endif

#pragma GCC visibility pop

#endif //VMCFG_AOT
