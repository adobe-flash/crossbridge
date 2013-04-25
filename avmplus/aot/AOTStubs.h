/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "CdeclThunk.h"
#include "AOTCompiler.h"

#ifdef DEBUGGER
#include "avmplusDebugger.h"
#endif

#define STUBS_VERBOSE 0

#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
#define DOUBLE_ALLOCA_UNUSED , ((double *)g_unusedParam)
#define DOUBLE_ALLOCA , doubleAlloca
#define DOUBLE_ALLOCA_DEF , double *doubleAlloca
#define DOUBLE_ALLOCA_DECL , double *
#else
#define DOUBLE_ALLOCA_UNUSED /**/
#define DOUBLE_ALLOCA /**/
#define DOUBLE_ALLOCA_DEF /**/
#define DOUBLE_ALLOCA_DECL /**/
#endif


//------------------------------------------------------------------------------
// Release with asserts
//------------------------------------------------------------------------------
#if 0
#ifdef AvmAssert
#undef AvmAssert
#endif

#ifdef AvmAssertMsg
#undef AvmAssertMsg
#endif

namespace avmplus {
    inline void _AvmAssertMsg(int32_t condition, const char* message) {
        if (!condition) {
            printf("%s\n", message);
            VMPI_debugBreak();
        }
    }

#define AvmAssert(condition) \
        _AvmAssert((condition), __LINE__,__FILE__)

#define AvmAssertMsg(condition,message) \
        do { avmplus::_AvmAssertMsg((condition), (message)); } while (0) /* no semi */

#define _AvmAssert(condition, line_, file_) \
        __AvmAssert((condition), line_, file_)

#define __AvmAssert(condition, line_, file_) \
        do { AvmAssertMsg((condition), \
                "Assertion failed: \"" #condition "\" (" #file_ ":" #line_ ")\n"); \
        } while (0) /* no semi */

}
#endif
//------------------------------------------------------------------------------

using namespace avmplus;

#pragma GCC visibility push(hidden)

//------------------------------------------------------------------------------
// Template conditionals
//------------------------------------------------------------------------------

namespace {
    template<typename t_To, typename t_From> struct conversion_helper {
        typedef char Small;
        struct Big {
            char dummy[2];
        };
        static Big Test(...);
        static Small Test(t_To);
        static t_From MakeFrom();
    };

    template<bool isSuperSub, typename t_To, typename t_From> struct convert_helper {
        static inline t_To convert(t_From) {
            AvmAssert(false);
            t_To* dummy = 0;
            return *dummy;
        }
    };

    template<bool isSuperSub, typename t_To> struct convert_helper<isSuperSub, t_To *, void *> {
        static inline t_To *convert(void *p) {
            return static_cast<t_To *> (p);
        }
    };

    template<typename t_To, typename t_From> struct convert_helper<true, t_To, t_From> {
        static inline t_To convert(t_From f) {
            return f;
        }
    };

    template<bool isSuperSub, typename T> struct convert_helper<isSuperSub, T, T> {
        static inline T convert(T t) {
            return t;
        }
    };

    template<bool isSuperSub, typename T> struct convert_helper<isSuperSub, void, T> {
        static inline void convert(T) {
            AvmAssert(false);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, void, LLVMUnusedParam> {
        static inline void convert(LLVMUnusedParam) {
        }
    };

    template<bool isSuperSub, typename T> struct convert_helper<isSuperSub, T, void> {
        static inline T convert(void) {
            AvmAssert(false);
            T* dummy = 0;
            return *dummy;
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, void, void> {
        static inline void convert(void) {
            AvmAssert(false);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, uint32_t, int32_t> {
        static inline uint32_t convert(int32_t i) {
            return static_cast<uint32_t> (i);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, uint32_t, double> {
        static inline uint32_t convert(double d) {
            return static_cast<uint32_t> (AvmCore::integer_d(d));
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, int32_t, uint32_t> {
        static inline int32_t convert(uint32_t i) {
            return static_cast<int32_t> (i);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, int32_t, double> {
        static inline int32_t convert(double d) {
            return AvmCore::integer_d(d);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, double, uint32_t> {
        static inline double convert(uint32_t i) {
            return static_cast<double> (i);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, double, int32_t> {
        static inline double convert(int32_t i) {
            return static_cast<double> (i);
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, Atom, LLVMAtom> {
        static inline Atom convert(LLVMAtom a) {
            return (Atom) a;
        }
    };

    template<bool isSuperSub> struct convert_helper<isSuperSub, LLVMBool, bool> {
        static inline LLVMBool convert(bool b) {
            return (LLVMBool)(b ? 1 : 0);
        }
    };
}

// Forward declaration...
template<class t_Super, class t_Sub> struct super_sub_class_ptr;

// Used to determine if one type (U) can be converted to another type.
template<typename t_To, typename t_From> struct conversion {
private:
    typedef conversion_helper<t_To, t_From> Helper;
public:
    enum {
        exists = sizeof(typename Helper::Small) == sizeof((Helper::Test(Helper::MakeFrom())))
    };
    enum {
        exists2Way = exists && conversion<t_From, t_To>::exists
    };
    enum {
        eq = false
    };
    static inline t_To convert(t_From f) {
        return convert_helper<super_sub_class_ptr<t_To, t_From>::result, t_To, t_From>::convert(f);
    }
};

template<class T> struct conversion<T, T> {
    enum {
        exists = true, exists2Way = true, eq = true
    };
    static inline T convert(T t) {
        return t;
    }
};

template<class T> struct conversion<T, void> {
    enum {
        exists = false, exists2Way = false, eq = false
    };
    static inline T convert() {
        AvmAssert(false);
        T* dummy = 0;
        return *dummy;
    }
};

template<> struct conversion<void, LLVMUnusedParam> {
    enum {
        exists = false, exists2Way = false, eq = false
    };
    static inline void convert(LLVMUnusedParam) {
    }
};

template<class T> struct conversion<void, T> {
    enum {
        exists = false, exists2Way = false, eq = false
    };
    static inline void convert(T) {
        AvmAssert(false);
    }
};

template<> struct conversion<void, void> {
    enum {
        exists = true, exists2Way = true, eq = true
    };
    static inline void convert(void) {
        AvmAssert(false);
    }
};

template<typename t_From> struct conversion_string {
public:
    static inline String* convert(t_From) {
        AvmAssert(false);
        return NULL;
    }
};

template<> struct conversion_string<String*> {
public:
    static inline String* convert(String* s) {
        return s;
    }
};

template<typename t_From> struct conversion_numeric {
public:
    static inline int32_t convert(t_From) {
        AvmAssert(false);
        return -1;
    }
};

template<> struct conversion_numeric<int32_t> {
public:
    static inline int32_t convert(int32_t n) {
        return n;
    }
};

template<> struct conversion_numeric<uint32_t> {
public:
    static inline uint32_t convert(uint32_t n) {
        return n;
    }
};

template<> struct conversion_numeric<double> {
public:
    static inline double convert(double n) {
        return n;
    }
};

template<typename t1> struct is_integer {
    enum {
        yes = (conversion<t1, int32_t>::eq || conversion<t1, uint32_t>::eq)
    };
};

template<typename t1, typename t2> struct both_integer {
    enum {
        yes = (is_integer<t1>::yes && is_integer<t2>::yes)
    };
};

template<typename t1> struct is_numeric {
    enum {
        yes = (is_integer<t1>::yes || conversion<t1, double>::eq)
    };
};

template<typename t1, typename t2> struct both_numeric {
    enum {
        yes = (is_numeric<t1>::yes && is_numeric<t2>::yes)
    };
};

template<typename t1, typename t2> struct both_boolean {
    enum {
        yes = (conversion<t1, LLVMBool>::eq && conversion<t2, LLVMBool>::eq)
    };
};

template<typename T> struct type_traits {
private:
    template <class U> struct un_const { typedef U result; enum { isConst = false }; };
    template <class U> struct un_const<const U> { typedef U result; enum { isConst = true }; };
    template <class U> struct un_const<const U&> { typedef U result; enum { isConst = true }; };

    template <class U> struct un_volatile { typedef U result; enum { isVolatile = false }; };
    template <class U> struct un_volatile<volatile U> { typedef U result; enum { isVolatile = true }; };
    template <class U> struct un_volatile<volatile U&> { typedef U result; enum { isVolatile = true }; };

    struct bad_type;
    template <class U> struct ptr_traits { enum { result = false }; typedef bad_type pointee; };
    template <class U> struct ptr_traits<U*> { enum { result = true }; typedef U pointee; };
    template <class U> struct ptr_traits<U*&> { enum { result = true }; typedef U pointee; };
public:
    typedef typename un_volatile<typename un_const<T>::result>::result unqualified_type;
    typedef typename ptr_traits<unqualified_type>::pointee pointee;

    enum { isPointer = ptr_traits<T>::result };
};

// evaluates to true iff t_SuperPtr is a pointer type, t_SubPtr is a pointer type, type_traits<t_SuperPtr>::pointee is a public
// base class of type_traits<t_SubPtr>::pointee.
template <class t_SuperPtr, class t_SubPtr> struct super_sub_class_ptr
{
private:
    template <class T> struct make_ptr_const_volatile {};
    template <class T> struct make_ptr_const_volatile<T*> { typedef const volatile T* result; };

    template <class B, class D> struct helper { enum { result = false }; };
    template <class B, class D> struct helper<B*, D*>
    {
    private:
        typedef typename make_ptr_const_volatile<B*>::result B1;
        typedef typename make_ptr_const_volatile<D*>::result D1;
    public:
        enum { result = conversion<B1, D1>::exists && !conversion<B1, const volatile void*>::eq };
    };

public:
    enum { result = helper<typename type_traits<t_SuperPtr>::unqualified_type, typename type_traits<t_SubPtr>::unqualified_type>::result };
};

template<typename T> struct type_void {
    enum { eq = false };
    static T assert_void()
    {
        AvmAssert(false);
        T* dummy = 0;
        return *dummy;
    }
};

template<> struct type_void<void> {
    enum { eq = true };
    static void assert_void()
    {
    }
};

template<typename T> bool type_known_to_be_compatible(MethodEnv *, Traits *) {
    return false;
}

template<> extern bool type_known_to_be_compatible<LLVMAtom> (MethodEnv *, Traits *t);
template<> extern bool type_known_to_be_compatible<int32_t> (MethodEnv *env, Traits *t);
template<> extern bool type_known_to_be_compatible<uint32_t> (MethodEnv *env, Traits *t);
template<> extern bool type_known_to_be_compatible<double> (MethodEnv *env, Traits *t);
template<> extern bool type_known_to_be_compatible<LLVMBool> (MethodEnv *env, Traits *t);
template<> extern bool type_known_to_be_compatible<Stringp> (MethodEnv *env, Traits *t);
template<> extern bool type_known_to_be_compatible<void> (MethodEnv *env, Traits *t);
template<> extern bool type_known_to_be_compatible<ScriptObject *> (MethodEnv *env, Traits *t);

template <typename T> const char *typeName()
{
    if(conversion<int32_t, T>::eq) return "int32_t";
    if(conversion<uint32_t, T>::eq) return "uint32_t";
    if(conversion<double, T>::eq) return "double";
    if(conversion<Stringp, T>::eq) return "Stringp";
    if(conversion<ScriptObject*, T>::eq) return "ScriptObject*";
    if(conversion<ArrayObject*, T>::eq) return "ArrayObject*";
    return "*unknown*";
}

//------------------------------------------------------------------------------
// Boxing
//------------------------------------------------------------------------------

template<typename rt, typename t1> rt abcOP_box(MethodEnv *env, t1 a);

template<> extern LLVMAtom abcOP_box<LLVMAtom, LLVMBool> (MethodEnv *, LLVMBool b);
template<> extern LLVMAtom abcOP_box<LLVMAtom, ScriptObject*> (MethodEnv *, ScriptObject* o);
template<> extern LLVMAtom abcOP_box<LLVMAtom, ArrayObject*> (MethodEnv *, ArrayObject* o);
template<> extern LLVMAtom abcOP_box<LLVMAtom, DoubleVectorObject*>(MethodEnv *, DoubleVectorObject* o);
template<> extern LLVMAtom abcOP_box<LLVMAtom, IntVectorObject*>(MethodEnv *, IntVectorObject* o);
template<> extern LLVMAtom abcOP_box<LLVMAtom, UIntVectorObject*>(MethodEnv *, UIntVectorObject* o);
template<> extern LLVMAtom abcOP_box<LLVMAtom, ObjectVectorObject*>(MethodEnv *, ObjectVectorObject* o);
template<> extern LLVMAtom abcOP_box<LLVMAtom, String*> (MethodEnv *, String* s);
template<> extern LLVMAtom abcOP_box<LLVMAtom, int32_t> (MethodEnv *env, int32_t i);
template<> extern LLVMAtom abcOP_box<LLVMAtom, uint32_t> (MethodEnv *env, uint32_t i);
template<> extern LLVMAtom abcOP_box<LLVMAtom, double> (MethodEnv *env, double d);
template<> extern LLVMAtom abcOP_box<LLVMAtom, QNameObject*> (MethodEnv *, QNameObject* q);
template<> extern LLVMAtom abcOP_box<LLVMAtom, Namespace*> (MethodEnv *, Namespace* n);
template<> extern LLVMAtom abcOP_box<LLVMAtom, LLVMUnusedParam> (MethodEnv *, LLVMUnusedParam);

//------------------------------------------------------------------------------

template<> extern void abcOP_box(MethodEnv *, LLVMAtom);
template<> extern LLVMAtom abcOP_box(MethodEnv *, LLVMAtom x);
template<> extern LLVMBool abcOP_box(MethodEnv *, LLVMBool x);
template<> extern ScriptObject* abcOP_box(MethodEnv *, ScriptObject* x);
template<> extern ArrayObject* abcOP_box(MethodEnv *, ArrayObject* x);
template<> extern ScriptObject* abcOP_box(MethodEnv *, ArrayObject* x);
template<> extern ArrayObject* abcOP_box(MethodEnv *, ScriptObject* x);
//template <> extern String* abcOP_box(MethodEnv *env, ScriptObject* x);
template<> extern String* abcOP_box(MethodEnv *, String* x);
template<> extern int32_t abcOP_box(MethodEnv *, int32_t x);
template<> extern int32_t abcOP_box(MethodEnv *, LLVMBool x);
template<> extern uint32_t abcOP_box(MethodEnv *, uint32_t x);
template<> extern double abcOP_box(MethodEnv *, double x);
template<> extern QNameObject* abcOP_box(MethodEnv *, QNameObject* x);
template<> extern Namespace* abcOP_box(MethodEnv *, Namespace* x);
template<> extern double abcOP_box(MethodEnv *, int32_t x);

//------------------------------------------------------------------------------
// Un-Boxing
//------------------------------------------------------------------------------

template<typename rt, typename t1> rt abcOP_unbox(MethodEnv *env, t1 a);

template<> extern ScriptObject* abcOP_unbox<ScriptObject*, Atom> (MethodEnv *, Atom a);
template<> extern ArrayObject* abcOP_unbox<ArrayObject*, Atom> (MethodEnv *, Atom a);
template<> extern DoubleVectorObject* abcOP_unbox<DoubleVectorObject*, Atom>(MethodEnv *, Atom a);
template<> extern IntVectorObject* abcOP_unbox<IntVectorObject*, Atom>(MethodEnv *, Atom a);
template<> extern UIntVectorObject* abcOP_unbox<UIntVectorObject*, Atom>(MethodEnv *, Atom a);
template<> extern ObjectVectorObject* abcOP_unbox<ObjectVectorObject*, Atom>(MethodEnv *, Atom a);
template<> extern String* abcOP_unbox<String*, Atom> (MethodEnv *, Atom a);
template<> extern String* abcOP_unbox<String*, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern int32_t abcOP_unbox<int32_t, Atom> (MethodEnv *, Atom a);
template<> extern LLVMAtom abcOP_unbox<LLVMAtom, Atom> (MethodEnv *, Atom a);
template<> extern LLVMBool abcOP_unbox<LLVMBool, Atom> (MethodEnv *, Atom a);
template<> extern Namespace* abcOP_unbox<Namespace*, Atom> (MethodEnv *, Atom a);
template<> extern uint32_t abcOP_unbox<uint32_t, Atom> (MethodEnv *, Atom a);
template<> extern QNameObject* abcOP_unbox<QNameObject*, Atom> (MethodEnv *, Atom a);
template<> extern double abcOP_unbox<double, Atom> (MethodEnv *, Atom a);
template<> extern void abcOP_unbox<void, Atom> (MethodEnv *, Atom);

//------------------------------------------------------------------------------

template<> extern ScriptObject* abcOP_unbox<ScriptObject*, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern ArrayObject* abcOP_unbox<ArrayObject*, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern DoubleVectorObject* abcOP_unbox<DoubleVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a);
template<> extern IntVectorObject* abcOP_unbox<IntVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a);
template<> extern UIntVectorObject* abcOP_unbox<UIntVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a);
template<> extern ObjectVectorObject* abcOP_unbox<ObjectVectorObject*, LLVMAtom>(MethodEnv *, LLVMAtom a);
template<> extern int32_t abcOP_unbox<int32_t, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern LLVMAtom abcOP_unbox<LLVMAtom, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern LLVMBool abcOP_unbox<LLVMBool, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern Namespace* abcOP_unbox<Namespace*, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern uint32_t abcOP_unbox<uint32_t, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern QNameObject* abcOP_unbox<QNameObject*, LLVMAtom> (MethodEnv *, LLVMAtom a);
template<> extern double abcOP_unbox<double, LLVMAtom> (MethodEnv *, LLVMAtom a);

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

bool doubleIsUint32(double n, uint32_t *out);
bool atomIsUint32(Atom a, uint32_t *out);
bool doubleIsInt32(double n, int32_t *out);
bool atomIsInt32(Atom a, int32_t *out);

template <typename T> inline bool isNullOrUndefined(MethodEnv* env, T v);
template <> inline bool isNullOrUndefined(MethodEnv*, LLVMBool) { return false; } 
template <> inline bool isNullOrUndefined(MethodEnv*, ScriptObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, ArrayObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, DoubleVectorObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, IntVectorObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, UIntVectorObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, ObjectVectorObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, String* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, Namespace* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, QNameObject* v) { return v == NULL; } 
template <> inline bool isNullOrUndefined(MethodEnv*, int32_t) { return false; } 
template <> inline bool isNullOrUndefined(MethodEnv*, uint32_t) { return false; } 
template <> inline bool isNullOrUndefined(MethodEnv*, double) { return false; } 
template <> inline bool isNullOrUndefined(MethodEnv*, LLVMAtom v) { return AvmCore::isNullOrUndefined((Atom)v); } 

// Never inline these since they're rarely actually called, and we'd prefer smaller code.
// We also prefer to pass a MethodEnv argument which is always handy, unlike a Toplevel which often is not.
void throwNull(MethodEnv* env) __attribute__((noreturn, noinline));
void throwNullOrUndefined(MethodEnv*, Atom atom) __attribute__((noreturn, noinline));

template<typename T> inline void nullcheck(MethodEnv* env, T v) {
    if (isNullOrUndefined(env, v))
        throwNull(env);
}

template<> inline void nullcheck(MethodEnv* env, LLVMAtom v) // only Atoms can be undefined
{
    if (isNullOrUndefined(env, v))
        throwNullOrUndefined(env, (Atom) v);
}

template <typename T> void abcOP_nullcheck(MethodEnv* env, T v)
{
    nullcheck(env, v);
}

template<typename rt>
rt abcOP_null(MethodEnv* env, Traits* traits) {
    // null script object
    if (super_sub_class_ptr<ScriptObject*, rt>::result || conversion<String*, rt>::eq || conversion<QNameObject*, rt>::eq || conversion<Namespace*, rt>::eq) {
        return conversion<rt, void*>::convert(NULL);
    }
    if (conversion<double, rt>::eq || conversion<int32_t, rt>::eq || conversion<uint32_t, rt>::eq) {
        return conversion<rt, uint32_t>::convert(0);
    } else {
        AvmCore* core = env->core();
        AvmAssert(VOID_TYPE != NULL);
        if (traits == VOID_TYPE)
            return abcOP_unbox<rt> (env, nullObjectAtom);
        else if (traits == STRING_TYPE)
            return abcOP_unbox<rt> (env, nullStringAtom);
        /*else if (traits == NAMESPACE_TYPE)
         return abcOP_unbox<rt>(env, nullNsAtom);*/// Toplevel::coerce appears to return nullObjectAtom for NAMESPACE_TYPE
        else
            return abcOP_unbox<rt> (env, nullObjectAtom);
    }
}

template<typename rt>
rt abcOP_nullExceptIfVoid(MethodEnv* env, Traits* traits) {
    if (conversion<LLVMAtom, rt>::eq) {
        AvmCore* core = env->core();
        AvmAssert(VOID_TYPE != NULL);
        if (traits == VOID_TYPE)
            return abcOP_unbox<rt> (env, undefinedAtom);
    }
    return abcOP_null<rt> (env, traits);
}

//------------------------------------------------------------------------------
// OP_coerce_s
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_coerce_s(MethodEnv *env, t1 val) {
    AvmCore *core = env->core();
    return abcOP_unbox<rt> (env, isNullOrUndefined(env, val) ? nullStringAtom : core->string((Atom) abcOP_box<LLVMAtom, t1> (env, val))->atom());
}

//------------------------------------------------------------------------------
// OP_coerce
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_coerce(MethodEnv *env, Traits** traitsID, t1 val) {
    AvmAssert(traitsID != NULL);
    Traits* traits = *traitsID;

    if (super_sub_class_ptr<ScriptObject*, t1>::result) {
        if (!conversion<ScriptObject*, t1>::convert(val))
            return abcOP_nullExceptIfVoid<rt> (env, traits);
    }

    if (isNullOrUndefined(env, val))
        return abcOP_nullExceptIfVoid<rt> (env, traits);

    Atom atom = (Atom) abcOP_box<LLVMAtom, t1> (env, val);

    if (!traits) {
        // type from the future...
        env->toplevel()->throwTypeError(kCheckTypeFailedError, env->core()->atomToErrorString(atom));
        // unreachable
    }
    return abcOP_unbox<rt> (env, env->toplevel()->coerce(atom, traits));
}

template<> extern String* abcOP_coerce<String*, LLVMAtom> (MethodEnv *env, Traits**, LLVMAtom val);
template<> extern LLVMBool abcOP_coerce<LLVMBool, LLVMAtom> (MethodEnv *env, Traits**, LLVMAtom val);
template<> extern int32_t abcOP_coerce<int32_t, LLVMAtom> (MethodEnv *env, Traits**, LLVMAtom val);
template<> extern uint32_t abcOP_coerce<uint32_t, LLVMAtom> (MethodEnv *env, Traits**, LLVMAtom val);
template<> extern double abcOP_coerce<double, LLVMAtom> (MethodEnv *env, Traits**, LLVMAtom val);

//------------------------------------------------------------------------------
// OP_coerce_a
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_coerce_a(MethodEnv *env, t1 val) {
    if (conversion<rt, t1>::eq)
        return conversion<rt, t1>::convert(val);
    return abcOP_unbox<rt> (env, abcOP_box<LLVMAtom, t1> (env, val));
}

//------------------------------------------------------------------------------
// OP_coerce_o
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_coerce_o(MethodEnv *env, t1 val) {
    Atom a = (Atom) abcOP_box<LLVMAtom, t1> (env, val);
    return abcOP_unbox<rt> (env, (LLVMAtom)((a == undefinedAtom) ? nullObjectAtom : a));
}

//------------------------------------------------------------------------------
// OP_convert_ns
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_convert_ns(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, env->internRtns((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// Multiname helper functions
//------------------------------------------------------------------------------

void applyQNameToMultiName(Multiname &mn, QNameObject* qn);

template<typename tNameIndex, typename t1, typename t2>
const Multiname *abcOP_setup_multiname(MethodEnv *env, Multiname &multiname, tNameIndex multinameIndex, t1 name, t2 ns) {

    if(conversion<Multiname *, tNameIndex>::eq || conversion<LLVMMultinamePtrMaybeInt, tNameIndex>::eq) // LLVMEmitter provided us with a live multiname!
    {
        Multiname *multinamep;

        if(conversion<Multiname *, tNameIndex>::eq)
            multinamep = conversion<Multiname *, tNameIndex>::convert(multinameIndex);
        else 
            multinamep = (Multiname *)conversion<LLVMMultinamePtrMaybeInt, tNameIndex>::convert(multinameIndex);

        if(conversion<LLVMUnusedParam, t1>::eq && conversion<LLVMUnusedParam, t2>::eq)
            return multinamep; // not runtime? just return it
        else
            multiname = *multinamep; // runtime? copy it
    } else {
        aotGetPool(env)->parseMultiname(multiname, (int) multinameIndex); // TODO should be using conversion<>
    }

    AvmCore *core = env->core();

    if (super_sub_class_ptr<QNameObject*, t1>::result) {
        QNameObject* qName = conversion<QNameObject*, t1>::convert(name);
        applyQNameToMultiName(multiname, qName);
        return &multiname;
    } else if ((super_sub_class_ptr<ScriptObject*, t1>::result) && (conversion<ScriptObject*, t1>::convert(name)) && (conversion<ScriptObject*, t1>::convert(name)->traits() == core->traits.qName_itraits)) {
        QNameObject* qName = static_cast<QNameObject*> (conversion<ScriptObject*, t1>::convert(name));
        applyQNameToMultiName(multiname, qName);
        return &multiname;
    } else if ((conversion<LLVMAtom, t1>::eq) && (AvmCore::isObject(conversion<Atom, t1>::convert(name)))) {
        ScriptObject* nameSO = AvmCore::atomToScriptObject(conversion<Atom, t1>::convert(name));
        if (nameSO->traits() == core->traits.qName_itraits) {
            QNameObject* qName = static_cast<QNameObject*> (nameSO);
            applyQNameToMultiName(multiname, qName);
            return &multiname;
        }
    }
    if (conversion<String*, t1>::eq)
        multiname.setName(env->core()->internString(conversion<String*, t1>::convert(name)));
    else if (!conversion<LLVMUnusedParam, t1>::eq)
        multiname.setName(env->core()->intern((Atom) abcOP_coerce_a<LLVMAtom> (env, name))); // TODO OPT ensure interned
    if (conversion<Namespace*, t2>::eq) {
        if (!ns)
            env->toplevel()->throwTypeError(kIllegalNamespaceError);
        multiname.setNamespace(env->core()->internNamespace(conversion<Namespace*, t2>::convert(ns)));
    } else if (!conversion<LLVMUnusedParam, t2>::eq) {
        multiname.setNamespace(abcOP_convert_ns<Namespacep, t2> (env, ns));
    }
    return &multiname;
}

//------------------------------------------------------------------------------
// OP_setcallee
//------------------------------------------------------------------------------
template<typename t1, typename t2>
void abcOP_setcallee(MethodEnv *env, t1 arrayval, t2 arg0val) {
    Atom array = (Atom) abcOP_box<LLVMAtom, t1> (env, arrayval);
    Atom arg0 = (Atom) abcOP_box<LLVMAtom, t2> (env, arg0val);
    ArrayObject *arguments = (ArrayObject *) AvmCore::atomToScriptObject(array);
    ScriptObject *closure;

    if (env->method->needClosure())
        closure = env->toplevel()->methodClosureClass()->create(env, arg0);
    else
        closure = ((FunctionEnv*) env)->closure;
    arguments->setStringProperty(env->core()->kcallee, closure->atom());
    arguments->setStringPropertyIsEnumerable(env->core()->kcallee, false);
}

//------------------------------------------------------------------------------
// OP_upcast
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_upcast(MethodEnv *env, t1 val) {
    if (conversion<rt, t1>::eq)
        return conversion<rt, t1>::convert(val);
    return abcOP_unbox<rt> (env, abcOP_box<LLVMAtom, t1> (env, val));
}

template<> extern String* abcOP_upcast(MethodEnv *env, ScriptObject* val);
template<> extern Namespace* abcOP_upcast(MethodEnv *env, ScriptObject* val);

//------------------------------------------------------------------------------
// OP_convert_i
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_convert_i(MethodEnv *env, t1 a) {
    if (is_numeric<rt>::yes) {
        if (is_numeric<t1>::yes)
            return conversion<rt, int32_t>::convert(conversion<int32_t, t1>::convert(a));

        if (conversion<LLVMAtom, t1>::eq) {
            Atom atom = conversion<Atom, t1>::convert(a);

            switch (atomKind(atom)) {
            case kIntptrType:
                return conversion<rt, int32_t>::convert(atomGetIntptr(atom));
            case kDoubleType:
                return conversion<rt, int32_t>::convert(conversion<int32_t, double>::convert(*(double *) atomPtr(atom)));
            default:
                return conversion<rt, int32_t>::convert(AvmCore::integer(atom));
            }
        }
    }
    return abcOP_unbox<rt> (env, env->core()->intAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// OP_convert_u
//------------------------------------------------------------------------------

template<typename rt, typename t1> inline
rt abcOP_convert_u(MethodEnv *env, t1 a) {
    if (is_numeric<rt>::yes) {
        if (is_numeric<t1>::yes)
            return conversion<rt, uint32_t>::convert(conversion<uint32_t, t1>::convert(a));
        if (conversion<LLVMAtom, t1>::eq) {
            Atom atom = conversion<Atom, t1>::convert(a);

            switch (atomKind(atom)) {
            case kIntptrType:
                return conversion<rt, uint32_t>::convert(atomGetIntptr(atom));
            case kDoubleType:
                return conversion<rt, uint32_t>::convert(conversion<uint32_t, double>::convert(*(double *) atomPtr(atom)));
            default:
                return conversion<rt, uint32_t>::convert(AvmCore::toUInt32(atom));
            }
        }
    }
    return abcOP_unbox<rt> (env, env->core()->uintAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// OP_convert_d
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_convert_d(MethodEnv *env, t1 a) {
    if (is_numeric<rt>::yes) {
        if (is_numeric<t1>::yes)
            return conversion<rt, double>::convert(conversion<double, t1>::convert(a));
        if (conversion<LLVMAtom, t1>::eq) {
            Atom atom = conversion<Atom, t1>::convert(a);

            switch (atomKind(atom)) {
            case kIntptrType:
                return conversion<rt, double>::convert(atomGetIntptr(atom));
            case kDoubleType:
                return conversion<rt, double>::convert(*(double *) atomPtr(atom));
            default:
                return conversion<rt, double>::convert(AvmCore::number(atom));
            }
        }
    }
    return abcOP_unbox<rt> (env, env->core()->numberAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// OP_convert_b
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_convert_b(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, (LLVMBool) env->core()->boolean((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

template<> extern LLVMBool abcOP_convert_b(MethodEnv *env, double d);
template<> extern LLVMBool abcOP_convert_b(MethodEnv *env, uint32_t u);
template<> extern LLVMBool abcOP_convert_b(MethodEnv *env, int32_t i);

//------------------------------------------------------------------------------
// OP_convert_s
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_convert_s(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, env->core()->string((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// OP_convert_o
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_convert_o(MethodEnv *env, t1 a) {
    nullcheck(env, a);
    Atom a_boxed = (Atom) abcOP_box<LLVMAtom, t1> (env, a);
    return abcOP_unbox<rt, Atom> (env, a_boxed);
}

//------------------------------------------------------------------------------
// Scopes
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_getglobalscope(MethodEnv *env) {
    return abcOP_unbox<rt> (env, env->scope()->getScope(0));
}

//------------------------------------------------------------------------------
// OP_new*
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_newactivation(MethodEnv *env) {
    ScriptObject* result = env->newUninitializedActivation();
    MethodEnv* init = result->vtable->init;
    if (init) {
        MethodInfo* initMI = init->method;
        AvmAssert(initMI->method_id() >= 0);
        AvmAssert(initMI->pool() == aotGetPool(env));
        const AOTInfo* aotInfo = initMI->pool()->aotInfo;
        AvmAssert(env->method->method_id() >= 0);
        AvmAssert(env->method->method_id() < aotInfo->nActivationTraits);
        uint32_t activationInitMethodId = aotInfo->activationInfo[env->method->method_id()].initMethodId;
        env->abcEnv()->setMethod(activationInitMethodId, init);
        init->coerceEnter(result->atom());
    }
    return abcOP_box<rt>(env, result);
}

template<typename rt, typename adt>
rt abcOP_newobject(MethodEnv *env, adt argDesc, ...) {

    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    int32_t nArgs = argDescArgCount(argDesc);

    AvmAssert(nArgs >= 0);

    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom* atomV = (Atom*) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * nArgs);
    argDescArgsToAtomv(atomV, argDesc, env, ap);
    ScriptObject *so = env->op_newobject( (nArgs-1) + atomV, nArgs / 2 );
    if(conversion<rt, ScriptObject *>::eq)
        return conversion<rt, ScriptObject *>::convert(so);
    return abcOP_unbox<rt>(env, so->atom() );
}

template<typename rt, typename adt>
rt abcOP_newarray(MethodEnv *env, adt argDesc, ...) {
    va_list ap;
    va_start(ap, argDesc); // FIXME va_end

    ArrayObject *array = env->toplevel()->arrayClass()->newArray(env, argDesc, ap);
    if(conversion<rt, ArrayObject *>::eq)
        return conversion<rt, ArrayObject *>::convert(array);
    if(conversion<rt, ScriptObject *>::eq)
        return conversion<rt, ScriptObject *>::convert((ScriptObject *)array);
    return abcOP_unbox<rt>(env, array->atom() );
}

template<typename rt>
rt abcOP_newcatch(MethodEnv *env, int32_t nameindex, char *u30typeindex) {
    Traits *t;

    if (!nameindex) {
        AvmCore *core = env->core();
        t = OBJECT_TYPE;
    } else {
        Multiname mn;
        PoolObject *pool = aotGetPool(env);
        pool->parseMultiname(mn, nameindex);
        t = Traits::newCatchTraits(env->toplevel(), pool, (TraitsPosPtr) u30typeindex, mn.getName(), mn.getNamespace());
    }
    return abcOP_box<rt> (env, env->newcatch(t));
}

template<typename rt>
rt abcOP_newfunction(MethodEnv *env, Traits** idForDeclaringTraits, Traits*** scopeTraitIds, uint32_t nScopeTraits, uint32_t nStateWithTraits, LLVMAtom *scopes_masked, uint32_t methodindex) {
    Atom *scopes = (Atom*) scopes_masked;
    PoolObject *pool = aotGetPool(env);
    AvmAssert(pool->methodCount() > methodindex);
    MethodInfo *body = pool->getMethodInfo(methodindex);
    AvmAssert(scopeTraitIds != NULL || (scopeTraitIds == NULL && nScopeTraits == 0));
    if (!(*idForDeclaringTraits)) {
        const AOTInfo* aotInfo = pool->aotInfo;
        AvmAssert(aotInfo != NULL);
        AvmAssert(body->declaringTraits() == 0);
        Toplevel* toplevel = env->toplevel();
        MMgc::GC::AllocaAutoPtr _scopeTraitsBuffer;
        Traits** scopeTraits = reinterpret_cast<Traits**> (VMPI_alloca(env->core(), _scopeTraitsBuffer, sizeof(Traits*) * nScopeTraits));
        for (uint32_t i = 0; i < nScopeTraits; ++i)
            scopeTraits[i] = scopeTraitIds[i] ? *(scopeTraitIds[i]) : NULL;

        const ScopeTypeChain* scopeTypeChain = ScopeTypeChain::create(aotGetGC(env), env->core()->traits.function_itraits, env->method->declaringScope(), scopeTraits, nScopeTraits, nStateWithTraits, NULL, NULL);
        body->makeIntoPrototypeFunction(toplevel, scopeTypeChain);
        Traits* declTraits = body->declaringTraits();
        *idForDeclaringTraits = declTraits;
        AvmAssert(declTraits != 0);
        // AvmAssert((toplevel->functionClass == 0) || (declTraits->base == toplevel->functionClass->ivtable()->traits));
        Traits* atraits = body->activationTraits();
        if (atraits) {
            aotInfo->activationTraits[methodindex] = atraits;
            AvmAssert(body->activationTraits()->init == NULL);
            AvmThunkNativeFunctionHandler handler = aotInfo->activationInfo[methodindex].initHandler;
            if (handler != NULL) {
                NativeMethodInfo compiledMethodInfo;
                compiledMethodInfo.thunker = aotThunker;
                AvmThunkNativeHandler nhandler;
                nhandler.function = handler;
                atraits->init = MethodInfo::create(aotGetGC(env), MethodInfo::kInitMethodStub, body->activationTraits(), &compiledMethodInfo, nhandler, aotInfo->activationInfo[methodindex].initMethodId);
            }

            // ---------------------------------------------------------------------------
            // Copied from Verifier::write() TODO: refactor so we can share this code
            const ScopeTypeChain *scope = body->activationScope();
            if (scope == NULL) {
                AvmAssert(atraits == body->activationTraits());
                scope = body->declaringScope()->cloneWithNewTraits(aotGetGC(env), atraits);
                atraits->setDeclaringScopes(scope);
                body->init_activationScope(scope);
            }
            // ---------------------------------------------------------------------------
        }
    }
    AvmAssert(body->declaringTraits() != 0);
    AvmAssert(*idForDeclaringTraits == body->declaringTraits());

    return abcOP_box<rt> (env, (ScriptObject*) env->newfunction(body, scopes));
}

template<typename rt>
rt abcOP_newclass(MethodEnv *env,
        Traits** idForCTraits,
        Traits** idForITraits,
        Traits*** scopeTraitIds,
        uint32_t nScopeTraits,
        uint32_t nStateWithTraits,
        LLVMAtom *scopes_masked,
        ScriptObject* basetype,
        uint32_t classindex)
{
    Atom *scopes = (Atom*) scopes_masked;
    Traits* ctraits = aotGetPool(env)->getClassTraits(classindex);
    AvmAssert(idForCTraits != 0);
    AvmAssert(idForITraits != 0);
    AvmAssert(scopeTraitIds != 0);
    if (!(*idForCTraits)) {
        Traits* itraits = ctraits->itraits;
        AvmAssert(itraits != 0);

        const AOTInfo* aotInfo = aotGetPool(env)->aotInfo;
        AvmAssert(itraits->m_interfaceBindingFunction == NULL);
        itraits->m_interfaceBindingFunction = aotInfo->interfaceBindingFunctions[classindex];

        MMgc::GC::AllocaAutoPtr _scopeTraitsBuffer;
        Traits** scopeTraits = reinterpret_cast<Traits**> (VMPI_alloca(env->core(), _scopeTraitsBuffer, sizeof(Traits*) * nScopeTraits));
        for (uint32_t i = 0; i < nScopeTraits; ++i) {
            AvmAssert(*(scopeTraitIds[i]) != 0);
            scopeTraits[i] = *(scopeTraitIds[i]);
        }

        const ScopeTypeChain* cscope = ScopeTypeChain::create(aotGetGC(env), ctraits, env->method->declaringScope(), scopeTraits, nScopeTraits, nStateWithTraits, NULL, ctraits);
        AvmAssert((nScopeTraits == 0) || ((*(scopeTraitIds[nScopeTraits - 1])) && ((*(scopeTraitIds[nScopeTraits - 1]))->itraits == itraits->base)));
        const ScopeTypeChain* iscope = ScopeTypeChain::create(aotGetGC(env), itraits, cscope, NULL, ctraits, itraits);
        Toplevel* toplevel = env->toplevel();
        ctraits->resolveSignatures(toplevel);
        itraits->resolveSignatures(toplevel);
        ctraits->setDeclaringScopes(cscope);
        itraits->setDeclaringScopes(iscope);

        *idForCTraits = ctraits;
        ctraits->initActivationTraits();
        *idForITraits = itraits;
        itraits->initActivationTraits();

        // TODO: Is this still really necessary?  Does the finddef force a necessary initializing side-effect?
        for (InterfaceIterator ifc_iter(itraits); ifc_iter.hasNext();) {
            Traits* intTraits = ifc_iter.next();
            if (intTraits && intTraits->isInterface()) {
                Multiname mn(intTraits->ns(), intTraits->name());
                env->finddef(&mn);
            }
        }
    }

    ScriptObject* o = basetype ? (ScriptObject*) (~7 & env->toplevel()->coerce(basetype->atom(), env->core()->traits.class_itraits)) : NULL;
    return abcOP_box<rt> (env, (ScriptObject*) env->newclass(ctraits, (ClassClosure*) o, env->scope(), scopes));
}

//------------------------------------------------------------------------------
// OP_findproperty
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex, typename t1, typename t2>
rt abcOP_findproperty(MethodEnv *env, tNameIndex multinameIndex, t1 n, t2 ns, LLVMAtom *scopes, int32_t scopeDepth, int32_t withDepth) {
    Multiname multiname;
    return abcOP_unbox<rt> (env, env->findproperty(env->scope(), (Atom*) scopes, scopeDepth, abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), false, (Atom*) scopes + scopeDepth - withDepth));
}

//------------------------------------------------------------------------------
// OP_findpropstrict
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex, typename t1, typename t2>
rt abcOP_findpropstrict(MethodEnv *env, tNameIndex multinameIndex, t1 n, t2 ns, LLVMAtom *scopes, int32_t scopeDepth, int32_t withDepth) {
    Multiname multiname;
    return abcOP_unbox<rt> (env, env->findproperty(env->scope(), (Atom*) scopes, scopeDepth, abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), true, (Atom*) scopes + scopeDepth - withDepth));
}

//------------------------------------------------------------------------------
// OP_finddef
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex>
rt abcOP_finddef(MethodEnv* env, tNameIndex multinameIndex) {
    Multiname multiname;
    return abcOP_box<rt> (env, env->finddef(abcOP_setup_multiname(env, multiname, multinameIndex, g_unusedParam, g_unusedParam)));
}

//------------------------------------------------------------------------------
// OP_getproperty, OP_setproperty, OP_delproperty, OP_hasproperty
//------------------------------------------------------------------------------

namespace {
    enum LLVMSelectGetSetDelHasProperty {
        LLVMSelectGet, LLVMSelectSet, LLVMSelectDel
    };
}

template<typename t1, typename t2>
bool isDictionaryLookup(t1 key, t2 obj) {
    if ((super_sub_class_ptr<ScriptObject*, t1>::result) || (conversion<LLVMAtom, t1>::eq && AvmCore::isObject(conversion<Atom, t1>::convert(key)))) {
        if (super_sub_class_ptr<ScriptObject*, t2>::result)
            return conversion<ScriptObject*, t2>::convert(obj)->traits()->isDictionary();
        else if (conversion<LLVMAtom, t2>::eq && AvmCore::isObject(conversion<Atom, t2>::convert(obj)))
            return AvmCore::atomToScriptObject(conversion<Atom, t2>::convert(obj))->traits()->isDictionary();
    }
    if (conversion<String *, t1>::eq && !conversion<String *, t1>::convert(key))
        return true;
    return false;
}

template <bool nc, typename objt>
VTable *abcOP_toVTable(MethodEnv *env, objt objp)
{
    VTable *vtable;

    if (super_sub_class_ptr<ScriptObject *, objt>::result)
    {
        if(nc)
            nullcheck(env, objp);
        vtable = conversion<ScriptObject *, objt>::convert(objp)->vtable;
    }
    else if(conversion<String *, objt>::eq)
    {
        if(nc)
            nullcheck(env, objp);
        vtable = env->toplevel()->stringClass()->ivtable();
    }
    else if(conversion<Namespace *, objt>::eq)
    {
        if(nc)
            nullcheck(env, objp);
        vtable = env->toplevel()->namespaceClass()->ivtable();
    }
    else if(conversion<LLVMBool, objt>::eq)
        vtable = env->toplevel()->booleanClass()->ivtable();
    else if(conversion<int32_t, objt>::eq || conversion<uint32_t, objt>::eq || conversion<double, objt>::eq)
        vtable = env->toplevel()->numberClass()->ivtable();
    else {
        Atom rcv = (Atom)abcOP_box<LLVMAtom, objt>(env, objp);
        vtable = env->toplevel()->toVTable(rcv); // does nullcheck
    }
    AvmAssert(vtable != NULL);
    return vtable;
}

// getproperty is willing to do conversion to int, uint, or Number as part of its operation
// doing conversions of this sort is currently unique to getproperty
template<typename rt>
rt convertForGetProperty(MethodEnv* env, Atom a)
{
    if(conversion<rt, int32_t>::eq)
        return abcOP_convert_i<rt, LLVMAtom>(env, (LLVMAtom)a);
    if(conversion<rt, uint32_t>::eq)
        return abcOP_convert_u<rt, LLVMAtom>(env, (LLVMAtom)a);
    if(conversion<rt, double>::eq)
        return abcOP_convert_d<rt, LLVMAtom>(env, (LLVMAtom)a);
    return abcOP_unbox<rt>(env, a);
}

template <bool nc, LLVMSelectGetSetDelHasProperty select, typename rt, typename t1, typename t2>
rt abcOP_get_set_del_has_property_impl(MethodEnv* env, const Multiname& multiname, t1 obj, t2 val) {
    Toplevel *toplevel = env->toplevel();
    Atom objAtom = (Atom) abcOP_box<LLVMAtom, t1> (env, obj);
    VTable* vtable = abcOP_toVTable<nc, t1>(env, obj);
    if (select == LLVMSelectGet)
    {
        return convertForGetProperty<rt>(env, toplevel->getproperty(objAtom, &multiname, vtable));
    }
    else if (select == LLVMSelectSet) {
        toplevel->setproperty(objAtom, &multiname, (Atom) abcOP_box<LLVMAtom, t2> (env, val), vtable);
        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
    } else {
        AvmAssert(select == LLVMSelectDel);
        return abcOP_unbox<rt> (env, env->delproperty(objAtom, &multiname));
    }
}

template <typename t>
struct is_known_derived_object_type
{
    enum
    {
        yes = conversion<t, ArrayObject *>::eq ||
        conversion<t, DoubleVectorObject *>::eq ||
        conversion<t, IntVectorObject *>::eq ||
        conversion<t, UIntVectorObject *>::eq
    };
};

template <typename objt>
bool isDoubleVector(objt obj)
{
    if(conversion<objt, DoubleVectorObject*>::eq)
        return true;
    if(is_known_derived_object_type<objt>::yes)
        return false;
    if(super_sub_class_ptr<ScriptObject*, objt>::result)
    {
        ScriptObject *so = conversion<ScriptObject *, objt>::convert(obj);
        Traits *traits = so->traits();
        AvmCore *core = traits->core;

        return traits == VECTORDOUBLE_TYPE;
    }
    return false;
}

template <typename objt>
bool isIntVector(objt obj)
{
    if(conversion<objt, IntVectorObject*>::eq)
        return true;
    if(is_known_derived_object_type<objt>::yes)
        return false;
    if(super_sub_class_ptr<ScriptObject*, objt>::result)
    {
        ScriptObject *so = conversion<ScriptObject *, objt>::convert(obj);
        Traits *traits = so->traits();
        AvmCore *core = traits->core;

        return traits == VECTORINT_TYPE;
    }
    return false;
}

template <typename objt>
bool isUIntVector(objt obj)
{
    if(conversion<objt, UIntVectorObject*>::eq)
        return true;
    if(is_known_derived_object_type<objt>::yes)
        return false;
    if(super_sub_class_ptr<ScriptObject*, objt>::result)
    {
        ScriptObject *so = conversion<ScriptObject *, objt>::convert(obj);
        Traits *traits = so->traits();
        AvmCore *core = traits->core;

        return traits == VECTORUINT_TYPE;
    }
    return false;
}

template <bool nc, LLVMSelectGetSetDelHasProperty select, typename rt, typename tNameIndex, typename t1, typename t2, typename t3, typename t4>
rt abcOP_get_set_del_has_property(MethodEnv* env DOUBLE_ALLOCA_DEF, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj, t4 val) {
    if (conversion<LLVMUnusedParam, t1>::eq && conversion<LLVMUnusedParam, t2>::eq) {
        Multiname multiname;
        const Multiname *multinamep = abcOP_setup_multiname(env, multiname, multinameIndex, n, ns);

        AvmAssert(!multinamep->isRuntime());
        // not runtime name or runtime ns
        return abcOP_get_set_del_has_property_impl<nc, select, rt, t3, t4> (env, *multinamep, obj, val);
    }
    if (conversion<LLVMUnusedParam, t2>::eq && (conversion<LLVMMultinameIndexMaybeInt, tNameIndex>::eq || conversion<LLVMMultinamePtrMaybeInt, tNameIndex>::eq)) {
        int32_t intIndex;

        // not runtime ns AND static ns is public ns.  Hello get/set/del/hasUintProperty.
        // TODO specialize uint index branch?
        if( (conversion<uint32_t, t1>::eq && ((int32_t)conversion<uint32_t, t1>::convert(n)) >= 0) || conversion<int32_t, t1>::eq ||
                (conversion<double, t1>::eq && doubleIsInt32(conversion<double, t1>::convert(n), &intIndex)) ||
                (conversion<LLVMAtom, t1>::eq && atomIsInt32(conversion<Atom, t1>::convert(n), &intIndex))) {
            // We statically know we can call the get/set/del/hasIntProperty if obj is a ScriptObject* or an Atom that happens to be
            // a ScriptObject.
            if (super_sub_class_ptr<ScriptObject*, t3>::result || (conversion<LLVMAtom, t3>::eq && AvmCore::isObject(conversion<Atom, t3>::convert(obj)))) {
                if(nc && super_sub_class_ptr<ScriptObject*, t3>::result && !conversion<ScriptObject *, t3>::convert(obj))
                    throwNull(env);

                ScriptObject* so = super_sub_class_ptr<ScriptObject*, t3>::result ?
                        conversion<ScriptObject*, t3>::convert(obj) : AvmCore::atomToScriptObject(conversion<Atom, t3>::convert(obj));

                if (!conversion<double, t1>::eq && !conversion<LLVMAtom, t1>::eq)
                    intIndex = conversion<int32_t, t1>::convert(n);

                // special case vectors with numeric gets/sets
                if((is_numeric<rt>::yes || conversion<rt, LLVMAtom>::eq) && select == LLVMSelectGet)
                {
                    if(isDoubleVector(obj))
                    {
                        double dres = ((DoubleVectorObject *)so)->_getNativeIntProperty(intIndex);

                        if(conversion<LLVMAtom, rt>::eq)
                        {
#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
                            *doubleAlloca = dres;
                            return conversion<rt, LLVMAtom>::convert((LLVMAtom)(kDoubleType | (uintptr_t)doubleAlloca));
#else
                            return abcOP_unbox<rt>(env, (Atom)abcOP_box<LLVMAtom, double>(env, dres));
#endif
                        }
                        if(conversion<double, rt>::eq)
                            return conversion<rt, double>::convert(dres);
                        return conversion<rt, int32_t>::convert((int32_t)dres);
                    }
                    if(isIntVector(obj))
                    {
                        int32_t res = ((IntVectorObject *)so)->_getNativeIntProperty(intIndex);

                        if(conversion<LLVMAtom, rt>::eq)
                        {
#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
                            *doubleAlloca = res; // TODO maybe check for int27?
                            return conversion<rt, LLVMAtom>::convert((LLVMAtom)(kDoubleType | (uintptr_t)doubleAlloca));
#else
                            return abcOP_unbox<rt>(env, (Atom)abcOP_box<LLVMAtom, int32_t>(env, res));
#endif
                        }
                        if(conversion<double, rt>::eq)
                            return conversion<rt, double>::convert((double)res);
                        return conversion<rt, int32_t>::convert(res);
                    }
                    if(isUIntVector(obj))
                    {
                        uint32_t ures = ((UIntVectorObject *)so)->_getNativeIntProperty(intIndex);

                        if(conversion<LLVMAtom, rt>::eq)
                        {
#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
                            *doubleAlloca = ures; // TODO maybe check for int27?
                            return conversion<rt, LLVMAtom>::convert((LLVMAtom)(kDoubleType | (uintptr_t)doubleAlloca));
#else
                            return abcOP_unbox<rt>(env, (Atom)abcOP_box<LLVMAtom, uint32_t>(env, ures));
#endif
                        }
                        if(conversion<double, rt>::eq)
                            return conversion<rt, double>::convert((double)ures);
                        return conversion<rt, uint32_t>::convert(ures);
                    }
                }
                else if(is_numeric<t4>::yes && select == LLVMSelectSet)
                {
                    if(isDoubleVector(obj))
                    {
                        double res = conversion<double, t4>::convert(val);
                       	if(((DoubleVectorObject *)so)->canTakeFastPath(intIndex))
							((DoubleVectorObject *)so)->_setNativeIntPropertyFast(intIndex,res);
						else
							((DoubleVectorObject *)so)->_setNativeIntPropertySlow(intIndex, res);
                        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
                    }
                    if(isIntVector(obj))
                    {
                        int32_t res = conversion<int32_t, t4>::convert(val);
                        	if(((IntVectorObject *)so)->canTakeFastPath(intIndex))
							((IntVectorObject *)so)->_setNativeIntPropertyFast(intIndex,res);
						else
							((IntVectorObject *)so)->_setNativeIntPropertySlow(intIndex, res);
						
                        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
                    }
                    if(isUIntVector(obj))
                    {
                        uint32_t res = conversion<uint32_t, t4>::convert(val);
						if(((UIntVectorObject *)so)->canTakeFastPath(intIndex))
							((UIntVectorObject *)so)->_setNativeIntPropertyFast(intIndex,res);
						else
							((UIntVectorObject *)so)->_setNativeIntPropertySlow(intIndex, res);
                        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
                    }
                }

                // array can specialize on int
                if(conversion<ArrayObject *, t3>::eq)
                {
                    ArrayObject *a = (ArrayObject *)so;

                    if (select == LLVMSelectGet) {
                        Atom res = a->_getIntProperty(intIndex);
                        if(conversion<rt, int32_t>::eq)
                            return abcOP_convert_i<rt, LLVMAtom>(env, (LLVMAtom)res);
                        if(conversion<rt, uint32_t>::eq)
                            return abcOP_convert_u<rt, LLVMAtom>(env, (LLVMAtom)res);
                        if(conversion<rt, double>::eq)
                            return abcOP_convert_d<rt, LLVMAtom>(env, (LLVMAtom)res);
                        return abcOP_unbox<rt>(env, res);
                    }
                    else if (select == LLVMSelectSet) {
                        a->_setIntProperty(intIndex, (Atom)abcOP_box<LLVMAtom, t4>(env, val));
                        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
                    }
                }

                //obj is a ScriptObject* just call getUIntProperty, verifying we're non-negative
                if(intIndex >= 0)
                {
                    uint32_t uintIndex = (uint32_t)intIndex;

                    if (select == LLVMSelectGet) {
                        Atom a = so->getUintProperty(uintIndex);
                        if(conversion<rt, int32_t>::eq)
                            return abcOP_convert_i<rt, LLVMAtom>(env, (LLVMAtom)a);
                        if(conversion<rt, uint32_t>::eq)
                            return abcOP_convert_u<rt, LLVMAtom>(env, (LLVMAtom)a);
                        if(conversion<rt, double>::eq)
                            return abcOP_convert_d<rt, LLVMAtom>(env, (LLVMAtom)a);
                        return abcOP_unbox<rt>(env, a);
                    }
                    else if (select == LLVMSelectSet) {
                        so->setUintProperty(uintIndex, (Atom) abcOP_box<LLVMAtom, t4> (env, val));
                        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
                    }
                    else {
                        AvmAssert(select == LLVMSelectDel);
                        return abcOP_unbox<rt, LLVMAtom> (env, (LLVMAtom)(so->delUintProperty(uintIndex) ? trueAtom : falseAtom));
                    }
                }
            }
            // obj is not a ScriptObject ( might be a String, uint, double, etc.... )
            // We'll want to fall through to toplevel->getpropery or getatomproperty
        }
    }

    Multiname multiname;

    if ((!conversion<LLVMUnusedParam, t2>::eq) || (!isDictionaryLookup(n, obj))) {
        if (select == LLVMSelectDel) {
            if ((conversion<LLVMAtom, t1>::eq) || (super_sub_class_ptr<ScriptObject*, t1>::result)) {
                Atom nAtom = (Atom) abcOP_box<LLVMAtom, t1> (env, n);
                if (AvmCore::isXMLList(nAtom))
                    env->toplevel()->throwTypeError(kDeleteTypeError, env->core()->toErrorString(env->toplevel()->toTraits(nAtom)));
            }
        }
        return abcOP_get_set_del_has_property_impl<nc, select, rt, t3, t4> (env, *abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), obj, val);
    }

    AvmAssert((conversion<LLVMUnusedParam, t2>::eq && (super_sub_class_ptr<ScriptObject*, t3>::result || conversion<LLVMAtom, t3>::eq) && isDictionaryLookup(n, obj)));

    Atom nameAtom = (Atom) abcOP_box<LLVMAtom, t1> (env, n);
    Atom objAtom = (Atom) abcOP_box<LLVMAtom, t3> (env, obj);
    VTable *objVTable = abcOP_toVTable<nc, t3>(env, obj);

    const Multiname *multinamep = abcOP_setup_multiname(env, multiname, multinameIndex, g_unusedParam, g_unusedParam);

    if(multinamep != &multiname)
        multiname = *multinamep;

    if (select == LLVMSelectGet) {
        return convertForGetProperty<rt> (env, env->getpropertyHelper(objAtom, &multiname, objVTable, nameAtom));
    } else if (select == LLVMSelectSet) {
        env->setpropertyHelper(objAtom, &multiname, (Atom) abcOP_box<LLVMAtom, t4> (env, val), objVTable, nameAtom);
        return conversion<rt, LLVMUnusedParam>::convert(g_unusedParam);
    } else {
        AvmAssert(select == LLVMSelectDel);
        return abcOP_unbox<rt> (env, env->delpropertyHelper(objAtom, &multiname, nameAtom));
    }
}

template<typename rt, typename tNameIndex, typename t1, typename t2, typename t3>
rt abcOP_getproperty(MethodEnv* env DOUBLE_ALLOCA_DEF, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj)
{
    return abcOP_get_set_del_has_property<true, LLVMSelectGet, rt, tNameIndex, t1, t2, t3, LLVMUnusedParam>(env DOUBLE_ALLOCA, multinameIndex, n, ns, obj, g_unusedParam);
}

template <typename rt, typename tNameIndex, typename t1, typename t2, typename t3>
rt abcOP_getproperty_nonc(MethodEnv* env DOUBLE_ALLOCA_DEF, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj)
{
    return abcOP_get_set_del_has_property<false, LLVMSelectGet, rt, tNameIndex, t1, t2, t3, LLVMUnusedParam>(env DOUBLE_ALLOCA, multinameIndex, n, ns, obj, g_unusedParam);
}

template <typename tNameIndex, typename t1, typename t2, typename t3, typename t4>
void abcOP_setproperty(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj, t4 val)
{
    abcOP_get_set_del_has_property<true, LLVMSelectSet, void, tNameIndex, t1, t2, t3, t4>(env DOUBLE_ALLOCA_UNUSED, multinameIndex, n, ns, obj, val);
}

template<typename tNameIndex, typename t1, typename t2, typename t3, typename t4>
void abcOP_setproperty_nonc(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj, t4 val)
{
    abcOP_get_set_del_has_property<false, LLVMSelectSet, void, tNameIndex, t1, t2, t3, t4>(env DOUBLE_ALLOCA_UNUSED, multinameIndex, n, ns, obj, val);
}

template<typename tNameIndex, typename t1, typename t2, typename t3>
LLVMBool abcOP_deleteproperty(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj)
{
    return abcOP_get_set_del_has_property<true, LLVMSelectDel, LLVMBool, tNameIndex, t1, t2, t3, LLVMUnusedParam>(env DOUBLE_ALLOCA_UNUSED, multinameIndex, n, ns, obj, g_unusedParam);
}

//------------------------------------------------------------------------------
// OP_getsuper
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex, typename t1, typename t2, typename t3>
rt abcOP_getsuper(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj) {
    Multiname multiname;
    return abcOP_unbox<rt> (env, env->getsuper((Atom) abcOP_box<LLVMAtom> (env, obj), abcOP_setup_multiname(env, multiname, multinameIndex, n, ns)));
}

//------------------------------------------------------------------------------
// OP_setsuper
//------------------------------------------------------------------------------

template<typename tNameIndex, typename t1, typename t2, typename t3, typename t4>
void abcOP_setsuper(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj, t4 val) {
    Multiname multiname;
    env->setsuper((Atom) abcOP_box<LLVMAtom> (env, obj), abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), (Atom) abcOP_box<LLVMAtom> (env, val));
}

//------------------------------------------------------------------------------
// OP_initproperty
//------------------------------------------------------------------------------

template<typename tNameIndex, typename t1, typename t2, typename t3, typename t4>
void abcOP_initproperty(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj, t4 val) {
    Multiname multiname;
    Atom objAtom = (Atom) abcOP_box<LLVMAtom, t3> (env, obj);
    VTable *vtable = abcOP_toVTable<true, t3>(env, obj);
    env->initproperty(objAtom, abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), (Atom) abcOP_box<LLVMAtom, t4> (env, val), vtable);
}

//------------------------------------------------------------------------------
// call helpers
//------------------------------------------------------------------------------

template<typename rt, typename adt>
rt callMethodEnvVA(MethodEnv *method, adt argDesc, va_list ap) {
    MethodInfo *info = method->method;
    AvmCore *core = method->core();

    if (info->isAotCompiled()) // fastest! going straight to compiled method
    {
#if STUBS_VERBOSE
        core->console << "callMethodEnvVA Straight to: " << info->getMethodName() << "\n";
#endif
        if (conversion<rt, double>::eq)
            return conversion<rt, double>::convert(coerceNCdeclArgDescEnter(argDesc, method, ap));
        else if (conversion<rt, int32_t>::eq)
            return conversion<rt, int32_t>::convert((int32_t) coerce32CdeclArgDescEnter(INT_TYPE, argDesc, method, ap));
        else if (conversion<rt, uint32_t>::eq)
            return conversion<rt, uint32_t>::convert((uint32_t) coerce32CdeclArgDescEnter(UINT_TYPE, argDesc, method, ap));
        else if (conversion<rt, LLVMBool>::eq)
            return conversion<rt, LLVMBool>::convert((LLVMBool) coerce32CdeclArgDescEnter(BOOLEAN_TYPE, argDesc, method, ap));
        else if (conversion<rt, Stringp>::eq)
            return conversion<rt, Stringp>::convert((Stringp) coerce32CdeclArgDescEnter(STRING_TYPE, argDesc, method, ap));
        else if (conversion<rt, ArrayObject *>::eq)
            return conversion<rt, ArrayObject *>::convert((ArrayObject *) coerce32CdeclArgDescEnter(ARRAY_TYPE, argDesc, method, ap));
        else if (type_void<rt>::eq)
            return coerce32CdeclArgDescEnter(VOID_TYPE, argDesc, method, ap), type_void<rt>::assert_void();
        else if (conversion<rt, LLVMAtom>::eq)
            return conversion<rt, LLVMAtom>::convert((LLVMAtom) coerce32CdeclArgDescEnter(NULL, argDesc, method, ap));
        else if (conversion<rt, QNameObject*>::eq)
            return conversion<rt, QNameObject*>::convert((QNameObject*) coerce32CdeclArgDescEnter(core->traits.qName_itraits, argDesc, method, ap));
        else {
            Traits *returnTraits = info->getMethodSignature()->returnTraits();
            ScriptObject* result = (ScriptObject *) coerce32CdeclArgDescEnter(returnTraits, argDesc, method, ap);
            AvmAssert(type_known_to_be_compatible<rt> (method, returnTraits) || (result == NULL));
            return conversion<rt, ScriptObject *>::convert(result);
        }
    }
#if STUBS_VERBOSE
    core->console << "callMethodEnvVA Not compiled: " << info->getMethodName() << " / " << hexAddr((uintptr_t)info) << "\n";
#endif
    int32_t size = argDescApSize(argDesc, method);
    MMgc::GC::AllocaAutoPtr _argsBuffer;
    uint32_t *args = (uint32_t *) VMPI_alloca(method->core(), _argsBuffer, size);
    int32_t argc = argDescArgsToAp(args, argDesc, method, ap);
    const MethodSignature *ms = info->getMethodSignature();

    if (!ms->argcOk(argc)) {
#if STUBS_VERBOSE
        core->console << "Function: " << info->getMethodName() << " requires " << ms->requiredParamCount() << " arguments, got " << argc << "\n";
#endif
        method->toplevel()->argumentErrorClass()->throwError(kWrongArgumentCountError, core->toErrorString(info), core->toErrorString(ms->requiredParamCount()), core->toErrorString(argc));
    }

    Traits *returnTraits = info->getMethodSignature()->returnTraits();

    if (type_known_to_be_compatible<rt> (method, returnTraits)) // second fastest -- use argc / ap calling convention without boxing return value
    {
#if STUBS_VERBOSE
        core->console << "callMethodEnvVA Ret type compatible callprop: " << info->getMethodName() << "\n";
#endif
        if (conversion<rt, double>::eq)
            return conversion<rt, double>::convert(method->implFPR()(method, argc, args));
        if (conversion<rt, int32_t>::eq)
            return conversion<rt, int32_t>::convert((int32_t) method->implGPR()(method, argc, args));
        if (conversion<rt, uint32_t>::eq)
            return conversion<rt, uint32_t>::convert((uint32_t) method->implGPR()(method, argc, args));
        if (conversion<rt, LLVMBool>::eq)
            return conversion<rt, LLVMBool>::convert((LLVMBool) method->implGPR()(method, argc, args));
        if (conversion<rt, Stringp>::eq)
            return conversion<rt, Stringp>::convert((Stringp) method->implGPR()(method, argc, args));
        if (conversion<rt, ArrayObject *>::eq)
            return conversion<rt, ArrayObject *>::convert((ArrayObject *) method->implGPR()(method, argc, args));
        else if (type_void<rt>::eq)
            return method->implGPR()(method, argc, args), type_void<rt>::assert_void();
        if (conversion<rt, LLVMAtom>::eq)
            return conversion<rt, LLVMAtom>::convert((LLVMAtom) method->implGPR()(method, argc, args));
        else
            return conversion<rt, ScriptObject *>::convert((ScriptObject *) method->implGPR()(method, argc, args));
    } else // third fastest -- use argc / ap calling convention with boxed(then unboxed) return value
    {
#if STUBS_VERBOSE
        core->console << "callMethodEnvVA Ret type incompatible callprop: " << info->getMethodName() << "\n";
#endif
        if (returnTraits == NUMBER_TYPE)
            return abcOP_unbox<rt> (method, core->doubleToAtom(method->implFPR()(method, argc, args)));
        else if (returnTraits == INT_TYPE)
            return abcOP_unbox<rt> (method, core->intToAtom((int32_t) method->implGPR()(method, argc, args)));
        else if (returnTraits == UINT_TYPE)
            return abcOP_unbox<rt> (method, core->uintToAtom((uint32_t) method->implGPR()(method, argc, args)));
        else if (returnTraits == BOOLEAN_TYPE)
            return abcOP_unbox<rt> (method, method->implGPR()(method, argc, args) ? trueAtom : falseAtom);
        else if (returnTraits == STRING_TYPE)
            return abcOP_unbox<rt> (method, ((Stringp) method->implGPR()(method, argc, args))->atom());
        else if (returnTraits == VOID_TYPE)
            return method->implGPR()(method, argc, args), abcOP_unbox<rt> (method, undefinedAtom);
        else if (Traits::isSSTAtom(returnTraits))
            return abcOP_unbox<rt> (method, (Atom) method->implGPR()(method, argc, args));
        else {
            AvmAssert(returnTraits->isSSTObject());
            return abcOP_unbox<rt> (method, ((ScriptObject *) method->implGPR()(method, argc, args))->atom());
        }
    }
}

//------------------------------------------------------------------------------
// OP_callproperty
//------------------------------------------------------------------------------
template<typename rt, typename objt, typename adt>
rt abcOP_callproperty_impl(MethodEnv* env, const Multiname &multiname, objt objp, adt argDesc, va_list ap) {
    Toplevel *toplevel = env->toplevel();
    VTable *vtable = abcOP_toVTable<true, objt>(env, objp);
    Binding b = toplevel->getBinding(vtable->traits, &multiname);

    if (AvmCore::bindingKind(b) == BKIND_METHOD) {
        MethodEnv *method = vtable->methods[AvmCore::bindingToMethodId(b)];
        return callMethodEnvVA<rt, adt> (method, argDesc, ap);
    }

#if STUBS_VERBOSE
    env->core()->console << "Slow callprop: " << multiname << "\n";
#endif

    // slowest -- atom parameters, atom result
    Atom rcv = (Atom)abcOP_box<LLVMAtom, objt>(env, objp);
    int32_t argc = argDescArgCount(argDesc);
    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom *atomv = (Atom *) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * argc);
    argDescArgsToAtomv(atomv, argDesc, env, ap);
    AvmAssert(argc > 0);
    return abcOP_unbox<rt> (env, toplevel->callproperty(rcv, &multiname, argc - 1, atomv, vtable));
}

template<typename rt, typename tNameIndex, typename t1, typename t2, typename objt, typename adt>
rt abcOP_callproperty(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, objt objp, adt argDesc, ...) {
    Multiname multiname;

    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    return abcOP_callproperty_impl<rt, objt, adt> (env, *abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), objp, argDesc, ap);
}

//------------------------------------------------------------------------------
// OP_call
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename adt>
rt abcOP_call(MethodEnv* env, t1 method, adt argDesc, ...) {
    Atom amethod = (Atom) abcOP_box<LLVMAtom, t1> (env, method);
    va_list ap;
    va_start(ap, argDesc); // FIXME va_end

#if STUBS_VERBOSE
    env->core()->console << "abcOP_call\n";
#endif
    if (!AvmCore::isObject(amethod))
        env->toplevel()->throwTypeErrorWithName(kCallOfNonFunctionError, "value");
#if 0 // TODO invalid optimization until we can set "this" in case of closure
    ScriptObject *so = AvmCore::atomToScriptObject(amethod);
    MethodEnv *methodEnv = so->getCallMethodEnv();

    if(methodEnv)
        return callMethodEnvVA<rt, adt>(methodEnv, argDesc, ap);
#endif

    // slowest -- atom parameters, atom result
    int32_t argc = argDescArgCount(argDesc);
    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom *atomv = (Atom *) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * argc);
    argDescArgsToAtomv(atomv, argDesc, env, ap);
    AvmAssert(argc > 0);
    return abcOP_unbox<rt> (env, avmplus::op_call(env, (Atom) abcOP_box<LLVMAtom, t1> (env, method), argc - 1, atomv));
}

template<typename rt>
rt abcOP_throwCallOfNonFunctionError(MethodEnv* env) /* __attribute__((noreturn)) */
{
    Stringp valueString = env->core()->toErrorString("value");
    env->toplevel()->throwTypeError(kCallOfNonFunctionError, valueString);    

    // Unreachable
    AvmAssert(false);
    return abcOP_unbox<rt> (env, undefinedAtom);
}

//------------------------------------------------------------------------------
// exact calling helper
//------------------------------------------------------------------------------

template <typename objt>
int32_t *abcOP_handlerFromMethodEnv(MethodEnv *env, objt objp)
{
    AvmAssert(env != NULL);
    union
    {
        AvmThunkNativeMethodHandler method;
        uint64_t value;
    } mv;

    mv.method = env->method->handler_method();
#if 0
    fprintf(stderr, "rmiv: %lld\n", mv.value);
#endif
#if defined(AVMPLUS_IA32)
    if(mv.value & 1) // entry in c++ vtable!
#else
        if((mv.value >> 32) == 1) // entry in c++ vtable!
#endif
        {
            ScriptObject *so;
            if(super_sub_class_ptr<ScriptObject *, objt>::result)
                so = conversion<ScriptObject *, objt>::convert(objp);
            else
            {
                Atom rcv = (Atom)abcOP_box<LLVMAtom, objt>(env, objp);
                so = AvmCore::atomToScriptObject(rcv);
            }
            int32_t **cppvt = *(int32_t ***)so;
            return cppvt[((uint32_t)mv.value) / sizeof(int *)];
        }
    return (int32_t *)(uintptr_t)mv.value;
}

//------------------------------------------------------------------------------
// OP_callmethod
//------------------------------------------------------------------------------

template <typename objt>
MethodEnv *abcOP_methodEnvFromDispId(MethodEnv *env, objt objp, int32_t methodindex)
{
    VTable *vtable = abcOP_toVTable<true, objt>(env, objp);
    MethodEnv *e = vtable->methods[methodindex-1];
    AvmAssert(e != NULL);
    return e;
}

template <typename objt>
MethodEnv *abcOP_methodEnvFromBaseDispId(MethodEnv *env, objt, int32_t methodindex) // TODO remove objt
{
    MethodEnv *e = env->vtable()->base->methods[methodindex-1];
    AvmAssert(e != NULL);
    return e;
}

template<typename rt, typename objt, typename adt>
rt abcOP_callmethod(MethodEnv* env, objt objp, int32_t methodindex, adt argDesc, ...) {
    va_list ap;
    va_start(ap, argDesc); // FIXME va_end	
    MethodEnv *f = abcOP_methodEnvFromDispId(env, objp, methodindex);

#if STUBS_VERBOSE
    env->core()->console << "abcOP_callmethod\n";
#endif
    int32_t argc = argDescArgCount(argDesc);
    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom *atomv = (Atom *)VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * argc);
    argDescArgsToAtomv(atomv, argDesc, f, ap);
    return abcOP_unbox<rt>(env, f->coerceEnter(argc-1, atomv));
}

//------------------------------------------------------------------------------
// OP_callstatic
//------------------------------------------------------------------------------

MethodEnv *abcOP_methodEnvFromIndex(AbcEnv *abcenv, int32_t methodindex);

template<typename rt, typename adt>
rt abcOP_callstatic(MethodEnv*, AbcEnv *abcenv, int32_t methodindex, adt argDesc, ...) {
    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    MethodEnv *f = abcOP_methodEnvFromIndex(abcenv, methodindex);

#if STUBS_VERBOSE
    env->core()->console << "abcOP_callstatic\n";
#endif
    return callMethodEnvVA<rt, adt> (f, argDesc, ap);
}

//------------------------------------------------------------------------------
// OP_callsuper
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex, typename t1, typename t2, typename adt>
rt abcOP_callsuper(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, adt argDesc, ...) {
    Multiname multiname;

#if STUBS_VERBOSE
    env->core()->console << "abcOP_callsuper_impl " << multiname << "\n";
#endif

    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    int32_t nArgs = argDescArgCount(argDesc);

    //This pointer means there will be at least on argument.
    AvmAssert(nArgs > 0);

    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom* atomV = (Atom*) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * nArgs);
    argDescArgsToAtomv(atomV, argDesc, env, ap);

    return abcOP_unbox<rt> (env, env->callsuper(abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), nArgs - 1, atomV));
}

//------------------------------------------------------------------------------
// OP_construct
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename adt>
rt abcOP_construct(MethodEnv* env, t1 obj, adt argDesc, ...) {
    Toplevel* toplevel = env->toplevel();
    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    int32_t nArgs = argDescArgCount(argDesc);

    //This pointer means there will be at least on argument.
    AvmAssert(nArgs > 0);

    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom* atomV = (Atom*) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * nArgs);
    argDescArgsToAtomv(atomV, argDesc, env, ap);
    Atom factoryAtom = (Atom) abcOP_box<LLVMAtom> (env, obj);
    return abcOP_unbox<rt> (env, toplevel->op_construct(factoryAtom, nArgs - 1, atomV));
}

void abcOP_constructsuper(MethodEnv* env, LLVMAtom *args, int32_t argc);

//------------------------------------------------------------------------------
// OP_constructprop
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex, typename t1, typename t2, typename adt>
rt abcOP_constructprop(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, adt argDesc, ...) {
    Multiname multiname;

    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    int32_t nArgs = argDescArgCount(argDesc);

    //This pointer means there will be at least on argument.
    AvmAssert(nArgs > 0);

    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom* atomV = (Atom*) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * nArgs);
    argDescArgsToAtomv(atomV, argDesc, env, ap);
    return abcOP_unbox<rt> (env, env->toplevel()->constructprop(abcOP_setup_multiname(env, multiname, multinameIndex, n, ns), nArgs - 1, atomV, env->toplevel()->toVTable(atomV[0])));
}

//------------------------------------------------------------------------------
// OP_getdescendants
//------------------------------------------------------------------------------

template<typename rt, typename tNameIndex, typename t1, typename t2, typename t3>
rt abcOP_getdescendants(MethodEnv* env, tNameIndex multinameIndex, t1 n, t2 ns, t3 obj) {
    Multiname multiname;

    return abcOP_unbox<rt> (env, env->getdescendants((Atom) abcOP_box<LLVMAtom, t3> (env, obj), abcOP_setup_multiname(env, multiname, multinameIndex, n, ns)));
}

//------------------------------------------------------------------------------
// applytype
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename adt>
rt abcOP_applytype(MethodEnv* env, t1 factory, adt argDesc, ...) {
    va_list ap;
    va_start(ap, argDesc); // FIXME va_end
    int32_t nArgs = argDescArgCount(argDesc);
    AvmAssert(nArgs > 0);
    MMgc::GC::AllocaAutoPtr _atomVBuffer;
    Atom* atomV = (Atom*) VMPI_alloca(env->core(), _atomVBuffer, sizeof(Atom) * nArgs);
    argDescArgsToAtomv(atomV, argDesc, env, ap);
    Atom factoryAtom = (Atom) abcOP_box<LLVMAtom> (env, factory);
    Atom atomResult = env->toplevel()->op_applytype(factoryAtom, nArgs, atomV);

    if ((nArgs == 1) && (env->toplevel()->toTraits(atomResult)->containsInterface(env->core()->traits.class_itraits)) && (env->toplevel()->toTraits(factoryAtom)->containsInterface(env->core()->traits.vector_ctraits))) {
        uint32_t i = 0;
        if (ISNULL(atomV[0])) {
            while ((i < abcNVectorInstances) && (abcVectorInstances[i].vectorParamTraitsId != 0))
                ++i;
        } else {
            Traits* parameterTraits = env->toplevel()->toTraits(atomV[0])->itraits;
            for (i = 0; i < abcNVectorInstances; ++i) {
                if ((abcVectorInstances[i].vectorParamTraitsId) && (*(abcVectorInstances[i].vectorParamTraitsId) == parameterTraits))
                    break;
            }
            AvmAssert((i >= abcNVectorInstances) || (*(abcVectorInstances[i].vectorParamTraitsId) == parameterTraits));
            AvmAssert((i >= abcNVectorInstances) || (abcVectorInstances[i].iTraits == NULL) || (abcVectorInstances[i].iTraits == env->toplevel()->toTraits(atomResult)->itraits));
        }
        if ((i < abcNVectorInstances) && (abcVectorInstances[i].iTraits == NULL))
            abcVectorInstances[i].iTraits = env->toplevel()->toTraits(atomResult)->itraits;
        AvmAssert((i >= abcNVectorInstances) || (abcVectorInstances[i].iTraits == env->toplevel()->toTraits(atomResult)->itraits));
    }
    return abcOP_unbox<rt> (env, atomResult);
}

//------------------------------------------------------------------------------
// OP_hasnext
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_hasnext(MethodEnv* env, t1 obj, t2 index) {
    return abcOP_box<rt, LLVMBool> (env, (LLVMBool) env->hasnext((Atom) abcOP_box<LLVMAtom, t1> (env, obj), abcOP_convert_i<int32_t, t2> (env, index)));
}

//------------------------------------------------------------------------------
// OP_hasnext2
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_hasnext2(avmplus::MethodEnv* env, t1 obj, t2 indexIn, int32_t* indexOut, LLVMAtom* objectOut) {
    Atom objAtom = (Atom) abcOP_box<LLVMAtom, t1> (env, obj);
    int indexInt = env->core()->integer((Atom) abcOP_box<LLVMAtom, t2> (env, indexIn));
    LLVMAtom hasNextResult = (LLVMAtom)(env->hasnextproto(objAtom, indexInt) ? trueAtom : falseAtom);
    *objectOut = (LLVMAtom) objAtom;
    *indexOut = indexInt;
    return abcOP_unbox<rt> (env, hasNextResult);
}

//------------------------------------------------------------------------------
// OP_nextname
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_nextname(MethodEnv* env, t1 obj, t2 index) {
    return abcOP_unbox<rt> (env, env->nextname((Atom) abcOP_box<LLVMAtom, t1> (env, obj), abcOP_convert_i<int32_t, t2> (env, index)));
}

//------------------------------------------------------------------------------
// OP_nextvalue
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_nextvalue(MethodEnv* env, t1 obj, t2 index) {
    return abcOP_unbox<rt> (env, env->nextvalue((Atom) abcOP_box<LLVMAtom, t1> (env, obj), abcOP_convert_i<int32_t, t2> (env, index)));
}

//------------------------------------------------------------------------------
// OP_not
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_not(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, (LLVMBool)(AvmCore::booleanAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a)) == falseAtom));
}

template<> extern LLVMBool abcOP_not(MethodEnv *, LLVMBool a);
template<> extern LLVMBool abcOP_not(MethodEnv *env, double d);
template<> extern LLVMBool abcOP_not(MethodEnv *env, uint32_t u);
template<> extern LLVMBool abcOP_not(MethodEnv *env, int32_t i);

// helper class that promotes ints to doubles when "rt" (representing the result type of an operation) is double
template<typename rt, typename t> struct promote_numeric {
	static int32_t convert(t)
	{
		AvmAssert(false);
		return -1;
	}
};

template<typename rt> struct promote_numeric<rt, int32_t> {
	static int32_t convert(int32_t v)
	{
		AvmAssert(is_numeric<rt>::yes);
		return v;
	}
};

template<typename rt> struct promote_numeric<rt, uint32_t> {
	static uint32_t convert(uint32_t v)
	{
		AvmAssert(is_numeric<rt>::yes);
		return v;
	}
};

template<typename rt> struct promote_numeric<rt, double> {
	static double convert(double v)
	{
		AvmAssert(is_numeric<rt>::yes);
		return v;
	}
};

template<> struct promote_numeric<double, int32_t> {
	static double convert(int32_t v)
	{
		return v;
	}
};

template<> struct promote_numeric<double, uint32_t> {
	static double convert(uint32_t v)
	{
		return v;
	}
};

template<> struct promote_numeric<double, double> {
	static double convert(double v)
	{
		return v;
	}
};

// used to demote numeric values based on a specified result type "rt" -- correctly handles AS3 style double=>int demotion
template<typename rt> struct demote_numeric {
	template<typename t> static rt convert(t v)
	{
		AvmAssert((both_numeric<rt, t>::yes));
		return conversion<rt, t>::convert(v);
	}
};

//------------------------------------------------------------------------------
// OP_add
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_add(MethodEnv *env DOUBLE_ALLOCA_DEF, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert(promote_numeric<rt, t1>::convert(a) + promote_numeric<rt, t2>::convert(b));
	
#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
    return abcOP_unbox<rt>(env, op_add(env->core(), doubleAlloca, (Atom)abcOP_box<LLVMAtom, t1>(env, a), (Atom)abcOP_box<LLVMAtom, t2>(env, b)));
#else
    return abcOP_unbox<rt> (env, op_add(env->core(), (Atom) abcOP_box<LLVMAtom, t1> (env, a), (Atom) abcOP_box<LLVMAtom, t2> (env, b)));
#endif
}

#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
template<typename rt, typename t1, typename t2>
rt abcOP_add(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert(promote_numeric<rt, t1>::convert(a) + promote_numeric<rt, t2>::convert(b));
	
    return abcOP_unbox<rt> (env, op_add(env->core(), (Atom) abcOP_box<LLVMAtom, t1> (env, a), (Atom) abcOP_box<LLVMAtom, t2> (env, b)));
}
#endif

//------------------------------------------------------------------------------
// OP_add_i
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_add_i(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert((int32_t)conversion_numeric<t1>::convert(a) + (int32_t)conversion_numeric<t2>::convert(b));
	
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) + abcOP_convert_i<int32_t, t2> (env, b));
}

//------------------------------------------------------------------------------
// OP_subtract
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_subtract(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert(promote_numeric<rt, t1>::convert(a) - promote_numeric<rt, t2>::convert(b));
	
    return abcOP_box<rt> (env, abcOP_convert_d<double, t1> (env, a) - abcOP_convert_d<double, t2> (env, b));
}

//------------------------------------------------------------------------------
// OP_subtract_i
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_subtract_i(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert((int32_t)conversion_numeric<t1>::convert(a) - (int32_t)conversion_numeric<t2>::convert(b));
	
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) - abcOP_convert_i<int32_t, t2> (env, b));
}

//------------------------------------------------------------------------------
// OP_multiply
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_multiply(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert(promote_numeric<rt, t1>::convert(a) * promote_numeric<rt, t2>::convert(b));
	
    return abcOP_box<rt> (env, abcOP_convert_d<double, t1> (env, a) * abcOP_convert_d<double, t2> (env, b));
}

//------------------------------------------------------------------------------
// OP_multiply_i
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_multiply_i(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert((int32_t)conversion_numeric<t1>::convert(a) * (int32_t)conversion_numeric<t2>::convert(b));
	
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) * abcOP_convert_i<int32_t, t2> (env, b));
}

//------------------------------------------------------------------------------
// OP_divide
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_divide(MethodEnv *env, t1 a, t2 b) {
	if(both_numeric<t1, t2>::yes && is_numeric<rt>::yes)
		return demote_numeric<rt>::convert(promote_numeric<rt, t1>::convert(a) / promote_numeric<rt, t2>::convert(b));
	
    return abcOP_box<rt> (env, abcOP_convert_d<double, t1> (env, a) / abcOP_convert_d<double, t2> (env, b));
}

//------------------------------------------------------------------------------
// OP_modulo
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_modulo(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, MathUtils::mod(abcOP_convert_d<double, t1> (env, a), abcOP_convert_d<double, t2> (env, b)));
}

//------------------------------------------------------------------------------
// OP_increment
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_increment(MethodEnv *env, t1 x) {
    if(is_numeric<t1>::yes)
	return abcOP_add<rt, t1, int32_t>(env, x, 1);
    return abcOP_add<rt, double, int32_t>(env, abcOP_convert_d<double, t1> (env, x), 1);
}

//------------------------------------------------------------------------------
// OP_decrement
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_decrement(MethodEnv *env, t1 x) {
	return abcOP_subtract<rt, t1, int32_t>(env, x, 1);
}

//------------------------------------------------------------------------------
// OP_increment_i
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_increment_i(MethodEnv *env, t1 x) {
	return abcOP_add_i<rt, t1, int32_t>(env, x, 1);
}

//------------------------------------------------------------------------------
// OP_decrement_i
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_decrement_i(MethodEnv *env, t1 x) {
	return abcOP_subtract_i<rt, t1, int32_t>(env, x, 1);
}

//------------------------------------------------------------------------------
// OP_bitand
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_bitand(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) & abcOP_convert_i<int32_t, t2> (env, b));
}

template<> extern int32_t abcOP_bitand(MethodEnv *, int32_t a, int32_t b);

//------------------------------------------------------------------------------
// OP_bitor
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_bitor(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) | abcOP_convert_i<int32_t, t2> (env, b));
}

template<> extern int32_t abcOP_bitor(MethodEnv *, int32_t a, int32_t b);

//------------------------------------------------------------------------------
// OP_bitxor
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_bitxor(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) ^ abcOP_convert_i<int32_t, t2> (env, b));
}

template<> extern int32_t abcOP_bitxor(MethodEnv *, int32_t a, int32_t b);

//------------------------------------------------------------------------------
// OP_bitnot
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_bitnot(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, ~abcOP_convert_i<int32_t, t1> (env, a));
}

template<> extern int32_t abcOP_bitnot(MethodEnv *, int32_t a);

//------------------------------------------------------------------------------
// OP_lshift
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_lshift(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) << (abcOP_convert_u<uint32_t, t2> (env, b) & 0x1F));
}

template<> extern int32_t abcOP_lshift(MethodEnv *, int32_t a, int32_t b);
template<> extern int32_t abcOP_lshift(MethodEnv *, double a, int32_t b);

//------------------------------------------------------------------------------
// OP_rshift
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_rshift(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, abcOP_convert_i<int32_t, t1> (env, a) >> (abcOP_convert_u<uint32_t, t2> (env, b) & 0x1F));
}

template<> extern int32_t abcOP_rshift(MethodEnv *, int32_t a, int32_t b);
template<> extern int32_t abcOP_rshift(MethodEnv *, double a, int32_t b);
template<> extern int32_t abcOP_rshift(MethodEnv *, uint32_t a, int32_t b);

//------------------------------------------------------------------------------
// OP_urshift
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_urshift(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, abcOP_convert_u<uint32_t, t1> (env, a) >> (abcOP_convert_u<uint32_t, t2> (env, b) & 0x1F));
}

template<> extern uint32_t abcOP_urshift(MethodEnv *, uint32_t a, uint32_t b);

//------------------------------------------------------------------------------
// OP_negate
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_negate(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, -abcOP_convert_d<double, t1> (env, a));
}

template<> extern int32_t abcOP_negate<int32_t, int32_t> (MethodEnv *, int32_t a);
template<> extern int32_t abcOP_negate<int32_t, double> (MethodEnv *, double a);
template<> extern double abcOP_negate<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// OP_negate_i
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_negate_i(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, -abcOP_convert_i<int32_t, t1> (env, a));
}

template<> extern int32_t abcOP_negate_i<int32_t, int32_t> (MethodEnv *, int32_t a);
template<> extern int32_t abcOP_negate_i<int32_t, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// OP_true
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_true(MethodEnv *env, t1 a) {
    return abcOP_convert_b<rt, t1> (env, a);
}

//------------------------------------------------------------------------------
// OP_compare_helper
//------------------------------------------------------------------------------

enum cmp_ops {
    cmp_lt,
    cmp_gt,
    cmp_lteq,
    cmp_gteq,
    cmp_eq,
    cmp_stricteq
};

enum math_ops {
    math_mul,
    math_mul_i,
    math_div,
    math_div_i,
    math_sub,
    math_sub_i,
    math_add,
    math_add_i
};

template <typename T> 
inline double toDouble(T v) { return double(v); }

template <>
inline double toDouble(LLVMBool v) { return double(uint(v)); }


/* 
 * When comparing same types that correspond to machine type, compare works fine, 
 * but when comparing different types it promotes to a type (double) capable of 
 * representing the entire domain of both types
 */
template<cmp_ops op, typename t1, typename t2>
bool cmp_func(t1 a_in, t2 b_in) {
    AvmAssert((both_numeric<t1, t2>::yes) || (both_boolean<t1, t2>::yes));
	
    double a = toDouble(a_in);
    double b = toDouble(b_in);
    switch(op){
    case cmp_lt:       return a < b;
    case cmp_gt:       return a > b;
    case cmp_lteq:     return a <= b;
    case cmp_gteq:     return a >= b;
    case cmp_eq:
    case cmp_stricteq: return a == b;
    default:
        AvmAssert(false);
        return false;
    }
}

template<cmp_ops op, typename t>
bool cmp_func(t a, t b) {
    AvmAssert((is_numeric<t>::yes) || (conversion<t, LLVMBool>::eq));
    switch(op){
    case cmp_lt:       return a < b;
    case cmp_gt:       return a > b;
    case cmp_lteq:     return a <= b;
    case cmp_gteq:     return a >= b;
    case cmp_eq:
    case cmp_stricteq: return a == b;
    default:
        AvmAssert(false);
        return false;
    }
}

template<cmp_ops op, typename rt, typename t1, typename t2>
rt abcOP_compare_helper(MethodEnv *env, t1 a, t2 b) {
    // Fast path for numeric comparisons
    if (both_numeric<t1, t2>::yes)
        return abcOP_unbox<rt> (env, cmp_func<op> (conversion_numeric<t1>::convert(a), conversion_numeric<t2>::convert(b)) ? trueAtom : falseAtom);

    // Fast path for boolean equality tests
    if ((op == cmp_eq || op == cmp_stricteq) && conversion<LLVMBool, t1>::eq && conversion<LLVMBool, t2>::eq)
        return abcOP_unbox<rt> (env, cmp_func<op, LLVMBool, LLVMBool> (conversion<LLVMBool, t1>::convert(a), conversion<LLVMBool, t2>::convert(b)) ? trueAtom : falseAtom);

    // Fast path for string comparisons
    if (conversion<String, t1>::eq && conversion<String, t2>::eq && !isNullOrUndefined<t1> (env, a) && !isNullOrUndefined<t2> (env, b)) {
        String *sa = conversion_string<t1>::convert(a);
        String *sb = conversion_string<t2>::convert(b);

        switch (op) {
        case cmp_lt:
            return abcOP_unbox<rt> (env, (*sa < *sb) ? trueAtom : falseAtom);
        case cmp_gt:
            return abcOP_unbox<rt> (env, (*sa > *sb) ? trueAtom : falseAtom);
        case cmp_gteq:
            return abcOP_unbox<rt> (env, (*sa >= *sb) ? trueAtom : falseAtom);
        case cmp_lteq:
            return abcOP_unbox<rt> (env, (*sa <= *sb) ? trueAtom : falseAtom);
        case cmp_eq:
        case cmp_stricteq:
            return abcOP_unbox<rt> (env, (sa == sb || *sa == *sb) ? trueAtom : falseAtom);
        }

        // The impossible has happened
        AvmAssert(false);
        return abcOP_unbox<rt> (env, undefinedAtom);
    }

    // Slow path for other comparisons
    Atom a_boxed = (Atom) abcOP_box<LLVMAtom, t1> (env, a);
    Atom b_boxed = (Atom) abcOP_box<LLVMAtom, t2> (env, b);
    switch (op) {
    case cmp_lt:
        return abcOP_unbox<rt> (env, env->core()->compare(a_boxed, b_boxed));
    case cmp_gt:
        return abcOP_unbox<rt> (env, env->core()->compare(b_boxed, a_boxed));
    case cmp_gteq:
        return abcOP_unbox<rt> (env, env->core()->compare(a_boxed, b_boxed) == falseAtom ? trueAtom : falseAtom);
    case cmp_lteq:
        return abcOP_unbox<rt> (env, env->core()->compare(b_boxed, a_boxed) == falseAtom ? trueAtom : falseAtom);
    case cmp_eq:
        return abcOP_unbox<rt> (env, env->core()->equals(a_boxed, b_boxed));
    case cmp_stricteq:
        return abcOP_unbox<rt> (env, env->core()->stricteq(a_boxed, b_boxed));
    }

    // The impossible has happened
    AvmAssert(false);
    return abcOP_unbox<rt> (env, undefinedAtom);
}

//------------------------------------------------------------------------------
// OP_equals
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_equals(MethodEnv *env, t1 a, t2 b) {
    return abcOP_compare_helper<cmp_eq, rt, t1, t2> (env, a, b);
}

//------------------------------------------------------------------------------
// OP_strictequals
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_strictequals(MethodEnv *env, t1 a, t2 b) {
    return abcOP_compare_helper<cmp_stricteq, rt, t1, t2> (env, a, b);
}

//------------------------------------------------------------------------------
// OP_lessthan
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_lessthan(MethodEnv *env, t1 a, t2 b) {
    return abcOP_compare_helper<cmp_lt, rt, t1, t2> (env, a, b);
}

//------------------------------------------------------------------------------
// OP_greaterthan
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_greaterthan(MethodEnv *env, t1 a, t2 b) {
    return abcOP_compare_helper<cmp_gt, rt, t1, t2> (env, a, b);
}

//------------------------------------------------------------------------------
// OP_greaterequals
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_greaterequals(MethodEnv *env, t1 a, t2 b) {
    return abcOP_compare_helper<cmp_gteq, rt, t1, t2> (env, a, b);
}

//------------------------------------------------------------------------------
// OP_lessequals
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_lessequals(MethodEnv *env, t1 a, t2 b) {
    return abcOP_compare_helper<cmp_lteq, rt, t1, t2> (env, a, b);
}

//------------------------------------------------------------------------------
// OP_istype
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_istype(MethodEnv *env, Traits** ppT, t1 a) {
    if (!ppT) // ANY
        return abcOP_unbox<rt> (env, trueAtom);

    Traits *t = *ppT;

    if (!t) // type from the future
        return abcOP_unbox<rt> (env, falseAtom);

    return abcOP_unbox<rt> (env, env->core()->istypeAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a), t));
}

//------------------------------------------------------------------------------
// OP_astype
//------------------------------------------------------------------------------

// ** TODO ** Why do we not use Traits** ( traitsID ) here.
template<typename rt, typename tNameIndex, typename t1>
rt abcOP_astype(MethodEnv *env, tNameIndex multinameIndex, t1 a) {
    Multiname multiname;
    const Multiname *multinamep = abcOP_setup_multiname(env, multiname, multinameIndex, g_unusedParam, g_unusedParam);

    // ---------------------------------------------------------------------------
    // Copied from Verifier::checkTypeName() TODO: refactor so we can share this code
    Toplevel *toplevel = env->toplevel();
    AvmCore *core = env->core();
    PoolObject *pool = aotGetPool(env);
    Traits* t = core->domainMgr()->findTraitsInPoolByMultiname(pool, *multinamep);

    if (t == NULL)
        toplevel->throwVerifyError(kClassNotFoundError, core->toErrorString(multinamep));
    if (t == (Traits*) BIND_AMBIGUOUS)
        toplevel->throwReferenceError(kAmbiguousBindingError, *multinamep);
    if (multiname.isParameterizedType())
        AvmAssert(false); // TODO: fixme
    // ---------------------------------------------------------------------------

    LLVMAtom atom = abcOP_box<LLVMAtom, t1> (env, a);
    return env->core()->istype((Atom) atom, t) ? abcOP_unbox<rt> (env, atom) : abcOP_null<rt> (env, t);
}

//------------------------------------------------------------------------------
// OP_istypelate
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_istypelate(MethodEnv *env, t1 a, t2 b) {
    Atom atom = (Atom) abcOP_box<LLVMAtom, t2> (env, b);
    // TODO can "b" be null?
    return abcOP_unbox<rt> (env, env->core()->istypeAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a), env->toplevel()->toClassITraits(atom)));
}

//------------------------------------------------------------------------------
// OP_astypelate
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_astypelate(MethodEnv *env, t1 val, t2 classobj) {
    // TODO can "classobj" be null?
    Traits *t = env->toplevel()->toClassITraits((Atom) abcOP_box<LLVMAtom, t2> (env, classobj));
    Atom atom = (Atom) abcOP_box<LLVMAtom, t1> (env, val);
    return env->core()->istype(atom, t) ? abcOP_unbox<rt> (env, atom) : abcOP_nullExceptIfVoid<rt> (env, t);
}

//------------------------------------------------------------------------------
// OP_typeof
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_typeof(MethodEnv* env, t1 a) {
    return abcOP_box<rt, String*> (env, env->core()->_typeof((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// OP_instanceof
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_instanceof(MethodEnv *env, t1 a, t2 b) {
    return abcOP_unbox<rt> (env, env->toplevel()->instanceof((Atom) abcOP_box<LLVMAtom, t1> (env, a), (Atom) abcOP_box<LLVMAtom, t2> (env, b)));
}

//------------------------------------------------------------------------------
// OP_in
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_in(MethodEnv *env, t1 a, t2 b) {
    return abcOP_unbox<rt> (env, env->toplevel()->in_operator((Atom) abcOP_box<LLVMAtom, t1> (env, a), (Atom) abcOP_box<LLVMAtom, t2> (env, b)));
}

//------------------------------------------------------------------------------
// OP_checkfilter
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_checkfilter(MethodEnv* env, t1 a) {
    env->checkfilter((Atom) abcOP_box<LLVMAtom, t1> (env, a));
}

//------------------------------------------------------------------------------
// OP_esc_xattr
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_esc_xattr(MethodEnv* env, t1 a) {
    return abcOP_box<rt> (env, env->core()->EscapeAttributeValue((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}

//------------------------------------------------------------------------------
// OP_esc_xelem
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_esc_xelem(MethodEnv* env, t1 a) {
    return abcOP_box<rt> (env, env->core()->ToXMLString((Atom) abcOP_box<LLVMAtom, t1> (env, a)));
}
//------------------------------------------------------------------------------
// OP_loadnull
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadnull(MethodEnv* env) {
    return abcOP_unbox<rt> (env, (LLVMAtom) nullObjectAtom);
}

//------------------------------------------------------------------------------
// OP_loadundefined
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadundefined(MethodEnv* env) {
    return abcOP_unbox<rt> (env, (LLVMAtom) undefinedAtom);
}

//------------------------------------------------------------------------------
// OP_loadnan
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadnan(MethodEnv* env) {
    return abcOP_unbox<rt> (env, (LLVMAtom) env->core()->kNaN);
}

template<> extern double abcOP_loadnan(MethodEnv* env);

//------------------------------------------------------------------------------
// OP_loadtrue
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadtrue(MethodEnv* env) {
    return abcOP_unbox<rt> (env, (LLVMAtom) trueAtom);
}

//------------------------------------------------------------------------------
// OP_loadfalse
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadfalse(MethodEnv* env) {
    return abcOP_unbox<rt> (env, (LLVMAtom) falseAtom);
}

//------------------------------------------------------------------------------
// OP_loadstring
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadstring(MethodEnv* env, uint32_t index) {
    return abcOP_box<rt> (env, aotGetString(env, index));
}

//------------------------------------------------------------------------------
// OP_loadnamespace
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_loadnamespace(MethodEnv* env, uint32_t index) {
    return abcOP_unbox<rt, Atom> (env, aotGetPool(env)->cpool_ns[index]->atom());
}

//------------------------------------------------------------------------------
// OP_getouterscope
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_getouterscope(MethodEnv* env, uint32_t index) {
    return abcOP_unbox<rt, Atom> (env, env->scope()->getScope(index));
}

//------------------------------------------------------------------------------
// OP_getslot
//------------------------------------------------------------------------------
template <typename st, typename t1>
REALLY_INLINE st* abcOP_getSlotPtr(MethodEnv* env, char* slotAddr, t1 obj)
{
    (void)env;
    (void)obj;
#ifdef DEBUG
    AvmAssert(!isNullOrUndefined<t1>(env, obj));
    ScriptObject* objPtr = conversion<ScriptObject*, t1>::convert(obj);
    AvmAssert(reinterpret_cast<uintptr_t>(objPtr) < reinterpret_cast<uintptr_t>(slotAddr));
    AvmAssert(((reinterpret_cast<uintptr_t>(objPtr) + env->toplevel()->toTraits((Atom)abcOP_box<LLVMAtom, t1>(env, obj))->getTotalSize()) >= reinterpret_cast<uintptr_t>(slotAddr) + sizeof(st)));
#endif
    st* castedSlotAddr = reinterpret_cast<st*>(slotAddr);
    return castedSlotAddr;
}

template <typename rt, typename t1>
REALLY_INLINE rt abcOP_getslot_impl(MethodEnv* env, char* slotAddr, t1 obj) {
    AvmAssert((super_sub_class_ptr<ScriptObject*, t1>::result));
    if (super_sub_class_ptr<ScriptObject*, t1>::result) {
        rt* slotPtr = abcOP_getSlotPtr<rt, t1>(env, slotAddr, obj);
        return *slotPtr;
    }
    return abcOP_unbox<rt>(env, undefinedAtom);
}

template <typename rt, typename t1>
REALLY_INLINE rt abcOP_getslot_nonc(MethodEnv* env, char* slotAddr, t1 obj) {
    return abcOP_getslot_impl<rt, t1>(env, slotAddr, obj);
}

template <typename rt, typename t1>
REALLY_INLINE rt abcOP_getslot(MethodEnv* env, char* slotAddr, t1 obj) {
    nullcheck(env, obj);
    return abcOP_getslot_impl<rt, t1>(env, slotAddr, obj);
}

//------------------------------------------------------------------------------
// OP_setslot
//------------------------------------------------------------------------------

template <typename T> REALLY_INLINE void* toVoidStar(T)
{
    AvmAssert(false);
    return 0;
}
template <typename T> REALLY_INLINE void* toVoidStar(T* t) {return t;}

template<typename t1, typename t2>
void abcOP_setslot_impl(MethodEnv* env, char* slotAddr, t1 obj, t2 val) {
    AvmAssert((super_sub_class_ptr<ScriptObject*, t1>::result));
    AvmAssert((super_sub_class_ptr<ScriptObject*, t1>::result));
    if (super_sub_class_ptr<ScriptObject*, t1>::result) {
        t2* slotPtr = abcOP_getSlotPtr<t2, t1> (env, slotAddr, obj);
        if ((super_sub_class_ptr<ScriptObject*, t2>::result) || (conversion<String*, t2>::eq) || (conversion<Namespace*, t2>::eq)) {
            WBRC(aotGetGC(env), obj, slotAddr, toVoidStar(val));
        } else if (conversion<LLVMAtom, t2>::eq) {
            WBATOM(aotGetGC(env), obj, ((Atom*) slotAddr), ((Atom) conversion<LLVMAtom, t2>::convert(val)));
        } else {
            AvmAssert((conversion<LLVMBool, t2>::eq || conversion<int32_t, t2>::eq || conversion<uint32_t, t2>::eq || conversion<double, t2>::eq));
            *slotPtr = val;
        }
    }
}

template <typename t1, typename t2>
REALLY_INLINE void abcOP_setslot_nonc(MethodEnv* env, char* slotAddr, t1 obj, t2 val) {
    abcOP_setslot_impl<t1, t2>(env, slotAddr, obj, val);
}

template <typename t1, typename t2>
REALLY_INLINE void abcOP_setslot(MethodEnv* env, char* slotAddr, t1 obj, t2 val) {
    nullcheck(env, obj);
    abcOP_setslot_impl<t1, t2>(env, slotAddr, obj, val);
}

//------------------------------------------------------------------------------
// OP_pushwith / OP_pushscope
//------------------------------------------------------------------------------

template<typename t1>
void abcOP_pushscope(MethodEnv* env, t1 val) {
    // meat is generated as LLVM operations... we're really a glorified null check
    nullcheck(env, val);
}

template<typename t1>
void abcOP_pushwith(MethodEnv* env, t1 val) {
    // meat is generated as LLVM operations... we're really a glorified null check
    nullcheck(env, val);
}

//------------------------------------------------------------------------------
// MethodFrame: DXNS and CodeContext support
//------------------------------------------------------------------------------

template<typename t1>
void abcOP_dxnslate(MethodEnv* env, AOTMethodFrame *methodFrame, t1 obj) {
    Namespace* dxns = env->core()->newPublicNamespace(abcOP_convert_s<String*, t1> (env, obj));
    methodFrame->set_dxns(dxns);
}

// More DXNS stuff can be found in AOTStubs.cpp.

//------------------------------------------------------------------------------
// Memory Ops
//------------------------------------------------------------------------------

#define MAKE_INTEGER(v)      ((intptr_t(v) << 3) | kIntptrType)

#define MOPS_RANGE_CHECK(addr, type) \
        if (addr < 0 || (uint32_t)((addr) + sizeof(type)) > env->domainEnv()->globalMemorySize()) { avmplus::mop_rangeCheckFailed(env); }

#define MOPS_LOAD(addr, type, result) \
        MOPS_RANGE_CHECK(addr, type) \
        result = *(const type*)(env->domainEnv()->globalMemoryBase() + (addr));

#define MOPS_STORE(addr, type, value) \
        MOPS_RANGE_CHECK(addr, type) \
        *(type*)(env->domainEnv()->globalMemoryBase() + (addr)) = (type)(value);

// probably should go elsewhere, but this is adequate for now.
#if defined(AVMPLUS_BIG_ENDIAN)
inline void _swap8(uint8_t& a, uint8_t& b)
{
    const uint8_t t = a;
    a = b;
    b = t;
}

inline void MOPS_SWAP_BYTES(uint16_t* p)
{
    union {
        uint16_t* pv;
        uint8_t* c;
    };
    pv = p;
    _swap8(c[0], c[1]);
}

inline void MOPS_SWAP_BYTES(int32_t* p)
{
    union {
        int32_t* pv;
        uint8_t* c;
    };
    pv = p;
    _swap8(c[0], c[3]);
    _swap8(c[1], c[2]);
}

inline void MOPS_SWAP_BYTES(float* p)
{
    union {
        float* pv;
        uint8_t* c;
    };
    pv = p;
    _swap8(c[0], c[3]);
    _swap8(c[1], c[2]);
}

inline void MOPS_SWAP_BYTES(double* p)
{
    union {
        double* pv;
        uint8_t* c;
    };
    pv = p;
    _swap8(c[0], c[7]);
    _swap8(c[1], c[6]);
    _swap8(c[2], c[5]);
    _swap8(c[3], c[4]);
}
#elif defined VMCFG_DOUBLE_MSW_FIRST
inline void MOPS_SWAP_BYTES(uint16_t*) {}
inline void MOPS_SWAP_BYTES(int32_t*) {}
inline void MOPS_SWAP_BYTES(float*) {}
inline void MOPS_SWAP_BYTES(double *p)
{
    union {
        double* pv;
        uint32_t* w;
    };
    pv = p;
    uint32_t t = w[0];
    w[0] = w[1];
    w[1] = t;
}
#else
#define MOPS_SWAP_BYTES(p) do {} while (0)
#endif

template<typename rt, typename t1>
rt abcOP_li8(MethodEnv* env, t1 a) {
    int32_t addr = abcOP_convert_i<int32_t, t1> (env, a);
    int32_t result = 0;
    MOPS_LOAD(addr, uint8_t, result);
    if (is_numeric<rt>::yes)
        return conversion<rt, int32_t>::convert(result);
    return abcOP_unbox<rt> (env, (LLVMAtom)MAKE_INTEGER(result));
}

template<typename rt, typename t1>
rt abcOP_li16(MethodEnv* env, t1 a) {
    int32_t addr = abcOP_convert_i<int32_t, t1> (env, a);
    int32_t result = 0;
    MOPS_LOAD(addr, uint16_t, result);
    MOPS_SWAP_BYTES(&result);
    if (is_numeric<rt>::yes)
        return conversion<rt, int32_t>::convert(result);
    return abcOP_unbox<rt> (env, (LLVMAtom)MAKE_INTEGER(result));
}

template<typename rt, typename t1>
rt abcOP_li32(MethodEnv* env, t1 a) {
    int32_t addr = abcOP_convert_i<int32_t, t1> (env, a);
    int32_t result = 0;
    MOPS_LOAD(addr, uint32_t, result);
    MOPS_SWAP_BYTES(&result);
    if (is_numeric<rt>::yes)
        return conversion<rt, int32_t>::convert(result);
    return abcOP_unbox<rt> (env, abcOP_box<LLVMAtom, int32_t> (env, result));
}

template<typename rt, typename t1>
rt abcOP_lf32(MethodEnv* env, t1 a) {
    int32_t addr = abcOP_convert_i<int32_t, t1> (env, a);
#ifndef VMCFG_UNALIGNED_FP_ACCESS
    // On iOS devices at least, VLDR.32 instructions must be aligned,
    // notwithstanding apparent claims to the contrary in the ARM documentation
    // (Watson bug #3168241)
    volatile union { float f; uint32_t u32; } u;
    MOPS_LOAD(addr, uint32_t, u.u32);
    float result = u.f;
#else
    float result = 0;
    MOPS_LOAD(addr, float, result);
#endif
    MOPS_SWAP_BYTES(&result);
    if (is_numeric<rt>::yes)
        return conversion<rt, double>::convert((double) result);
    return abcOP_unbox<rt> (env, abcOP_box<LLVMAtom, double> (env, (double) result));
}

template<typename rt, typename t1>
rt abcOP_lf64(MethodEnv* env, t1 a) {
    int32_t addr = abcOP_convert_i<int32_t, t1> (env, a);
#ifndef VMCFG_UNALIGNED_FP_ACCESS
    // On iOS devices at least, VLDR.64 instructions must be aligned,
    // notwithstanding apparent claims to the contrary in the ARM documentation
    // (Watson bug #3168241)
    volatile union { double d; uint64_t u64; } u;
    MOPS_LOAD(addr, uint64_t, u.u64);
    double result = u.d;
#else
    double result = 0;
    MOPS_LOAD(addr, double, result);
#endif
    
    MOPS_SWAP_BYTES(&result);
    if (is_numeric<rt>::yes)
        return conversion<rt, double>::convert(result);
    return abcOP_unbox<rt> (env, abcOP_box<LLVMAtom, double> (env, result));
}

template<typename t1, typename t2>
void abcOP_si8(MethodEnv* env, t1 a, t2 b) {
    uint8_t value = abcOP_convert_i<int32_t, t1> (env, a);
    int32_t addr = abcOP_convert_i<int32_t, t2> (env, b);
    MOPS_STORE(addr, uint8_t, value);
}

template<typename t1, typename t2>
void abcOP_si16(MethodEnv* env, t1 a, t2 b) {
    uint16_t value = abcOP_convert_i<int32_t, t1> (env, a);
    int32_t addr = abcOP_convert_i<int32_t, t2> (env, b);
    MOPS_SWAP_BYTES(&value);
    MOPS_STORE(addr, uint16_t, value);
}

template<typename t1, typename t2>
void abcOP_si32(MethodEnv* env, t1 a, t2 b) {
    uint32_t value = abcOP_convert_i<int32_t, t1> (env, a);
    int32_t addr = abcOP_convert_i<int32_t, t2> (env, b);
    MOPS_SWAP_BYTES(&value);
    MOPS_STORE(addr, uint32_t, value);
}

template<typename t1, typename t2>
void abcOP_sf32(MethodEnv* env, t1 a, t2 b) {
    double value = abcOP_convert_d<double, t1> (env, a);
    int32_t addr = abcOP_convert_i<int32_t, t2> (env, b);
    MOPS_SWAP_BYTES(&value);
#ifndef VMCFG_UNALIGNED_FP_ACCESS
    // On iOS at least, VSDR.32 instructions must be aligned,
    // notwithstanding apparent claims to the contrary in the ARM documentation.
    // (Watson bug #3168241)
    volatile union { float f; uint32_t u32; } u;
    u.f = value;
    MOPS_STORE(addr, uint32_t, u.u32);
#else
    MOPS_STORE(addr, float, value);
#endif
}

template<typename t1, typename t2>
void abcOP_sf64(MethodEnv* env, t1 a, t2 b) {
    double value = abcOP_convert_d<double, t1> (env, a);
    int32_t addr = abcOP_convert_i<int32_t, t2> (env, b);
    MOPS_SWAP_BYTES(&value);
#ifndef VMCFG_UNALIGNED_FP_ACCESS
    // On iOS at least, VSDR.64 instructions must be aligned,
    // notwithstanding apparent claims to the contrary in the ARM documentation.
    // (Watson bug #3168241)
    volatile union { double d; uint64_t u64; } u;
    u.d = value;
    MOPS_STORE(addr, uint64_t, u.u64);
#else
    MOPS_STORE(addr, double, value);
#endif
}

template<typename rt, typename t1>
rt abcOP_sxi1(MethodEnv* env, t1 a) {
    int32_t value = abcOP_convert_i<int32_t, t1> (env, a);
    if (is_numeric<rt>::yes)
        return conversion<rt, int32_t>::convert((value << (8 * sizeof(Atom) - 1)) >> (8 * sizeof(Atom) - 1));
    return abcOP_unbox<rt> (env, (LLVMAtom)(((value << (8 * sizeof(Atom) - 1)) >> ((8 * sizeof(Atom) - 1) - 3)) | kIntptrType));
}

template<typename rt, typename t1>
rt abcOP_sxi8(MethodEnv* env, t1 a) {
    int32_t value = abcOP_convert_i<int32_t, t1> (env, a);
    if (is_numeric<rt>::yes)
        return conversion<rt, int32_t>::convert((value << (8 * (sizeof(Atom) - 1))) >> (8 * (sizeof(Atom) - 1)));
    return abcOP_unbox<rt> (env, (LLVMAtom)(((value << (8 * (sizeof(Atom) - 1))) >> ((8 * (sizeof(Atom) - 1)) - 3)) | kIntptrType));
}

template<typename rt, typename t1>
rt abcOP_sxi16(MethodEnv* env, t1 a) {
    int32_t value = abcOP_convert_i<int32_t, t1> (env, a);
    if (is_numeric<rt>::yes)
        return conversion<rt, int32_t>::convert((value << (8 * (sizeof(Atom) - 2))) >> (8 * (sizeof(Atom) - 2)));
    return abcOP_unbox<rt> (env, (LLVMAtom)(((value << (8 * (sizeof(Atom) - 2))) >> ((8 * (sizeof(Atom) - 2)) - 3)) | kIntptrType));
}

#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
LLVMAtom abcOP_promoteOptimisticAtom(MethodEnv *env, LLVMAtom a);
#endif

//------------------------------------------------------------------------------
// Interface binding lookup
// bindings are represented as a series of length-prefixed runs of an interface
// id (minus the last interface's id if any) followed by a the run's "dispids"
//------------------------------------------------------------------------------

template<typename t1>
int32_t abcOP_findInterfaceBinding(int32_t iid, t1 rle) {
    while (true) {
        uint32_t runLength = *rle++;
        if (runLength == 0)
            return 0;
        iid -= *rle++;
        if (iid < 0)
            return 0;
        if (iid < runLength)
            return int32_t(rle[iid]);
        rle += runLength;
    }
}

//------------------------------------------------------------------------------
// OP_throw / exception stuff
//------------------------------------------------------------------------------

template<typename t1>
void abcOP_throw(MethodEnv* env, t1 a) {
#ifdef VMCFG_OPTIMISTIC_DOUBLE_ATOM
    env->core()->throwAtom((Atom)abcOP_promoteOptimisticAtom(env, abcOP_box<LLVMAtom, t1>(env, a)));
#else
    env->core()->throwAtom((Atom) abcOP_box<LLVMAtom, t1> (env, a));
#endif
}

template<typename rt>
rt abcOP_xarg(MethodEnv* env, ExceptionFrame* ef) {
    (void) ef;
    Exception *ex = env->core()->exceptionAddr;
    return abcOP_unbox<rt, Atom> (env, ex->atom);
}

void abcOP_beginTry(MethodEnv* env, ExceptionFrame* ef);
void abcOP_endTry(MethodEnv* env, ExceptionFrame* ef);
void abcOP_beginCatch(MethodEnv* env, ExceptionFrame* ef);
void abcOP_rethrow(MethodEnv* env, ExceptionFrame* ef);

//------------------------------------------------------------------------------
// OP_debug
//------------------------------------------------------------------------------

void abcOP_debugEnter(MethodEnv* env, CallStackNode* csn, LLVMAtom *vars, int32_t* exceptFilt);
void abcOP_debugExit(MethodEnv* env, CallStackNode* csn);
void abcOP_debug(MethodEnv* env, uint32_t i, uint32_t nameId, uint32_t slotNum, uint32_t lineNo);

//------------------------------------------------------------------------------
// OP_debugfile
//------------------------------------------------------------------------------
void abcOP_debugfile(MethodEnv* env, uint32_t index);

//------------------------------------------------------------------------------
// OP_debugline
//------------------------------------------------------------------------------
void abcOP_debugline(MethodEnv* env, uint32_t line);

//------------------------------------------------------------------------------
// "OP_Math_abs" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_abs(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::abs(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_abs<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_abs<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_acos" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_acos(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::acos(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_acos<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_acos<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_asin" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_asin(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::asin(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_asin<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_asin<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_atan" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_atan(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::atan(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_atan<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_atan<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_atan2" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_Math_atan2(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, MathUtils::atan2(abcOP_convert_d<double, t1> (env, a), abcOP_convert_d<double, t2> (env, b)));
}

template<> extern double abcOP_Math_atan2<double, int, int> (MethodEnv *, int32_t a, int32_t b);
template<> extern double abcOP_Math_atan2<double, double, double> (MethodEnv *, double a, double b);
template<> extern double abcOP_Math_atan2<double, int, double> (MethodEnv *, int32_t a, double b);
template<> extern double abcOP_Math_atan2<double, double, int> (MethodEnv *, double a, int32_t b);

//------------------------------------------------------------------------------
// "OP_Math_ceil" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_ceil(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::ceil(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_ceil<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_ceil<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_cos" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_cos(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::cos(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_cos<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_cos<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_exp" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_exp(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::exp(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_exp<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_exp<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_floor" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_floor(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::floor(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_floor<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_floor<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_log" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_log(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::log(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_log<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_log<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_pow" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_Math_pow(MethodEnv *env, t1 a, t2 b) {
    return abcOP_box<rt> (env, MathUtils::pow(abcOP_convert_d<double, t1> (env, a), abcOP_convert_d<double, t2> (env, b)));
}

template<> extern double abcOP_Math_pow<double, int, int> (MethodEnv *, int32_t a, int32_t b);
template<> extern double abcOP_Math_pow<double, double, double> (MethodEnv *, double a, double b);
template<> extern double abcOP_Math_pow<double, int, double> (MethodEnv *, int32_t a, double b);
template<> extern double abcOP_Math_pow<double, double, int> (MethodEnv *, double a, int32_t b);

//------------------------------------------------------------------------------
// "OP_Math_random" (Math)
//------------------------------------------------------------------------------

inline double abcOP_Math_random(MethodEnv*) {
    return ::random();
}

//------------------------------------------------------------------------------
// "OP_Math_round" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_round(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::round(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_round<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_round<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_sin" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_sin(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::sin(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_sin<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_sin<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_sqrt" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_sqrt(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::sqrt(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_sqrt<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_sqrt<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "OP_Math_tan" (Math)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_Math_tan(MethodEnv *env, t1 a) {
    return abcOP_box<rt> (env, MathUtils::tan(abcOP_convert_d<double, t1> (env, a)));
}

template<> extern double abcOP_Math_tan<double, int> (MethodEnv *, int32_t a);
template<> extern double abcOP_Math_tan<double, double> (MethodEnv *, double a);

//------------------------------------------------------------------------------
// "abcOP_String_charAt" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_String_charAt(MethodEnv *env, t1 a, t2 b) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    const double index = abcOP_convert_d<double, t2> (env, b);
    return abcOP_box<rt> (env, instance->AS3_charAt(index));
}

template<typename rt, typename t1>
rt abcOP_String_charAt(MethodEnv *env, t1 a) {
    return abcOP_String_charAt<rt, t1, double> (env, a, 0);
}

//------------------------------------------------------------------------------
// "abcOP_String_charCodeAt" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_String_charCodeAt(MethodEnv *env, t1 a, t2 b) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    const double index = abcOP_convert_d<double, t2> (env, b);
    return abcOP_box<rt> (env, instance->AS3_charCodeAt(index));
}

template<typename rt, typename t1>
rt abcOP_String_charCodeAt(MethodEnv *env, t1 a) {
    return abcOP_String_charCodeAt<rt, t1, double> (env, a, 0);
}

//------------------------------------------------------------------------------
// "abcOP_String_concat" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_String_concat(MethodEnv *env, t1 a, LLVMAtom argv, int32_t argc, ArrayObject*) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);

    AvmCore* core = env->core();
    Stringp concatString = instance;
    for (int32_t i = 0; i < argc; ++i)
        concatString = String::concatStrings(concatString, core->string(((Atom*)argv)[i]));

    return abcOP_box<rt> (env, concatString);
}

//------------------------------------------------------------------------------
// "abcOP_String_fromCharCode" (String, static)
//------------------------------------------------------------------------------

template<typename rt>
rt abcOP_String_fromCharCode(MethodEnv *env, LLVMAtom argv, int32_t argc, ArrayObject*) {
    return abcOP_box<rt> (env, env->toplevel()->stringClass()->fromCharCode((Atom*)argv, argc));
}

//------------------------------------------------------------------------------
// "abcOP_String_indexOf" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_indexOf(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Stringp val = abcOP_convert_s<Stringp, t2> (env, b);
    const double startIndex = abcOP_convert_d<double, t3> (env, c);
    return abcOP_box<rt> (env, instance->AS3_indexOf(val, startIndex));
}

template<typename rt, typename t1, typename t2>
rt abcOP_String_indexOf(MethodEnv *env, t1 a, t2 b) {
    return abcOP_String_indexOf<rt, t1, t2, int32_t> (env, a, b, 0);
}

//------------------------------------------------------------------------------
// "abcOP_String_lastIndexOf" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_lastIndexOf(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Stringp val = abcOP_convert_s<Stringp, t2> (env, b);
    const double startIndex = abcOP_convert_d<double, t3> (env, c);
    return abcOP_box<rt> (env, instance->AS3_lastIndexOf(val, startIndex));
}

template<typename rt, typename t1, typename t2>
rt abcOP_String_lastIndexOf(MethodEnv *env, t1 a, t2 b) {
    return abcOP_String_lastIndexOf<rt, t1, t2, double> (env, a, b, 0x7fffffff);
}

//------------------------------------------------------------------------------
// "abcOP_String_localeCompare" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_String_localeCompare(MethodEnv *env, t1 a, t2 b) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Stringp other = abcOP_convert_s<Stringp, t2> (env, b);

    return abcOP_box<rt> (env, other->Compare(*instance));
}

//------------------------------------------------------------------------------
// "abcOP_String_match" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_String_match(MethodEnv *env, t1 a, t2 b) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Atom pattern = (Atom) abcOP_box<LLVMAtom, t2> (env, b);
    nullcheck(env, pattern);
    return abcOP_box<rt> (env, env->toplevel()->stringClass()->_match(instance, pattern));
}

//------------------------------------------------------------------------------
// "abcOP_String_replace" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_replace(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Atom pattern = (Atom) abcOP_box<LLVMAtom, t2> (env, b);
    Atom replacementAtom = (Atom) abcOP_box<LLVMAtom, t3> (env, c);
    return abcOP_box<rt> (env, env->toplevel()->stringClass()->_replace(instance, pattern, replacementAtom));
}

//------------------------------------------------------------------------------
// "abcOP_String_search" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2>
rt abcOP_String_search(MethodEnv *env, t1 a, t2 b) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Atom pattern = (Atom) abcOP_box<LLVMAtom, t2> (env, b);
    nullcheck(env, pattern);
    return abcOP_box<rt> (env, env->toplevel()->stringClass()->_search(instance, pattern));
}

//------------------------------------------------------------------------------
// "abcOP_String_slice" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_slice(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    const double startIndex = abcOP_convert_d<double, t2> (env, b);
    const double endIndex = abcOP_convert_d<double, t3> (env, c);
    return abcOP_box<rt> (env, instance->AS3_slice(startIndex, endIndex));
}

template<typename rt, typename t1, typename t2>
rt abcOP_String_slice(MethodEnv *env, t1 a, t2 b) {
    return abcOP_String_slice<rt, t1, t2, double> (env, a, b, 0x7fffffff);
}

//------------------------------------------------------------------------------
// "abcOP_String_split" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_split(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    Atom delimiter = (Atom) abcOP_box<LLVMAtom, t2> (env, b);
    nullcheck(env, delimiter);
    uint32_t limit = abcOP_convert_u<uint32_t, t3> (env, c);

    // ECMA compatibility - limit can be undefined
    if (limit == 0x7fffffff)
        limit = 0xffffffff;

    return abcOP_box<rt> (env, env->toplevel()->stringClass()->_split(instance, delimiter, limit));
}

template<typename rt, typename t1, typename t2>
rt abcOP_String_split(MethodEnv *env, t1 a, t2 b) {
    return abcOP_String_split<rt, t1, t2, uint32_t> (env, a, b, 0xffffffff);
}

//------------------------------------------------------------------------------
// "abcOP_String_substr" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_substr(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    const double startIndex = abcOP_convert_d<double, t2> (env, b);
    const double len = abcOP_convert_d<double, t3> (env, c);
    return abcOP_box<rt> (env, instance->AS3_substr(startIndex, len));
}

template<typename rt, typename t1, typename t2>
rt abcOP_String_substr(MethodEnv *env, t1 a, t2 b) {
    return abcOP_String_substr<rt, t1, t2, double> (env, a, b, 0x7fffffff);
}

//------------------------------------------------------------------------------
// "abcOP_String_substring" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1, typename t2, typename t3>
rt abcOP_String_substring(MethodEnv *env, t1 a, t2 b, t3 c) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    const double startIndex = abcOP_convert_d<double, t2> (env, b);
    const double endIndex = abcOP_convert_d<double, t3> (env, c);
    return abcOP_box<rt> (env, instance->AS3_substring(startIndex, endIndex));
}

template<typename rt, typename t1, typename t2>
rt abcOP_String_substring(MethodEnv *env, t1 a, t2 b) {
    return abcOP_String_substring<rt, t1, t2, double> (env, a, b, 0x7fffffff);
}

//------------------------------------------------------------------------------
// "abcOP_String_toLocaleLowerCase" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_String_toLocaleLowerCase(MethodEnv *env, t1 a) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    return abcOP_box<rt> (env, instance->AS3_toLowerCase());
}

//------------------------------------------------------------------------------
// "abcOP_String_toLocaleUpperCase" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_String_toLocaleUpperCase(MethodEnv *env, t1 a) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    return abcOP_box<rt> (env, instance->AS3_toUpperCase());
}

//------------------------------------------------------------------------------
// "abcOP_String_toLowerCase" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_String_toLowerCase(MethodEnv *env, t1 a) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    return abcOP_box<rt> (env, instance->toLowerCase());
}

//------------------------------------------------------------------------------
// "abcOP_String_toUpperCase" (String)
//------------------------------------------------------------------------------

template<typename rt, typename t1>
rt abcOP_String_toUpperCase(MethodEnv *env, t1 a) {
    Stringp instance = abcOP_convert_s<Stringp, t1> (env, a);
    return abcOP_box<rt> (env, instance->toUpperCase());
}


#pragma GCC visibility pop
