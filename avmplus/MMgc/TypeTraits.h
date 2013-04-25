/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __TypeTraits__
#define __TypeTraits__

#include "StaticAssert.h"

/*
    This header file implements a small subset of the type traits
    functionality defined in TR1. At present, however, TR1 is not
    available on all supported systems.
*/

// Disable MSVC++ PREfast code analysis warnings triggered by template metaprogramming hackery.
#ifdef _PREFAST_
#pragma warning(push)
#pragma warning(disable:6334) // sizeof expression with operator may give unexpected result
#pragma warning(disable:6285) // result of '||' is always a non-zero constant
#endif

namespace MMgc
{
    template <typename _Type>
    struct remove_const { typedef _Type type; };

    template <typename _Type>
    struct remove_const<_Type const> { typedef _Type type; };

    template <typename _Type>
    struct remove_volatile { typedef _Type type; };

    template <typename _Type>
    struct remove_volatile<_Type volatile> { typedef _Type type; };

    template <typename _Type>
    struct remove_cv
    {
        typedef typename remove_const<typename remove_volatile<_Type>::type>::type type;
    };

    template <typename _Type>
    struct remove_pointer
    {
        typedef _Type type;
    };

#define _RP_SPECIALIZE(_ModifiedType) \
    template <typename _Type> \
    struct remove_pointer<_ModifiedType> \
    { \
        typedef _Type type; \
    };

    _RP_SPECIALIZE(_Type *)
    _RP_SPECIALIZE(_Type * const)
    _RP_SPECIALIZE(_Type * volatile)
    _RP_SPECIALIZE(_Type * const volatile)

#undef _RP_SPECIALIZE

    template <typename _Type, _Type _Value>
    struct integral_constant
    {
        static const _Type value = _Value;
        typedef _Type value_type;
        typedef integral_constant<_Type, _Value> type;
    };

    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;

    template <typename, typename>
    struct is_same : public false_type { };

    template <typename _Type>
    struct is_same<_Type, _Type> : public true_type { };

    template <typename _Type>
    struct _is_class_or_union_helper
    {
    private:
        typedef char _one;
        typedef struct { char _c[2]; } _two;

    public:
        template <typename _T>
            static _one _test (int _T::*);
        template <typename>
            static _two _test (...);

        static const bool _value = sizeof (_test<_Type>(0)) == sizeof (_one);
    };

    template <typename _Type>
    struct _is_class_or_union : public integral_constant<bool, _is_class_or_union_helper<_Type>::_value>
    {
    };

    template <typename _Base, typename _Derived>
    struct _is_base_of_helper
    {
    private:
        // Disallow incomplete types.
        MMGC_STATIC_ASSERT(sizeof (_Base) != 0);
        MMGC_STATIC_ASSERT(sizeof (_Derived) != 0);

        typedef typename remove_cv<_Base>::type _RCV_Base;
        typedef typename remove_cv<_Derived>::type _RCV_Derived;

        typedef char _one;
        typedef struct { char _c[2]; } _two;

    public:
        template <typename _T>
            static _one _test (_RCV_Derived &, _T);
        static _two _test (_RCV_Base &, int);

        struct _convert
        {
            operator _RCV_Base & () const;
            operator _RCV_Derived & ();
        };

        static const bool _value = (_is_class_or_union<_Base>::value &&
            _is_class_or_union<_Derived>::value &&
            (sizeof (_test(_convert(), 0)) == sizeof (_one) ||
            is_same<_Base, _Derived>::value));
    };

    template <typename _Base, typename _Derived>
    struct is_base_of :
        public integral_constant<bool, _is_base_of_helper<_Base, _Derived>::_value>
    {
    };
}

#ifdef _PREFAST_
#pragma warning(pop)
#endif

#endif /* __TypeTraits__ */
