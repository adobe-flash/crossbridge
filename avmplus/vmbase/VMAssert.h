/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __VmAssert__
#define __VmAssert__

//
// Use the standard assert
//
// Usage notes:
//    assert(condition);
//    assert((condition) && "message");
//    assert(!"message");
//

#if defined(DEBUG) || defined(_DEBUG)
    // turn off assert suppression
    #undef NDEBUG
#elif !defined(NDEBUG)
    // Define this AND LEAVE IT DEFINED to suppress asserts. It needs to stay defined because
    // other headers, notably system headers, may reinclude assert.h. According to the standard
    // and actual practice, assert.h can be included multiple times, and what happens each
    // time depends on whether NDEBUG is defined at that point.
    #define NDEBUG
#endif

#include <assert.h>
    
//
// Backfill static_assert for compilers that lack it
//
// Usage notes:
//    static_assert(condition, "message");
//

#define NEED_STATIC_ASSERT
#if _MSC_VER >= 1600  // Visual studio 10 has it
    #undef NEED_STATIC_ASSERT
#endif
#if __GXX_EXPERIMENTAL_CXX0X__ // GCC 4.3 has it, experimentally
    #undef NEED_STATIC_ASSERT
#endif

#ifdef NEED_STATIC_ASSERT
    #undef NEED_STATIC_ASSERT
    #undef static_assert

    namespace vmbase
    {
        template <int> struct static_assert_helper {};
        template <bool> struct STATIC_ASSERTION_FAILED;
        template <> struct STATIC_ASSERTION_FAILED<true> {};
    }

    #define _STATIC_ASSERT_JOIN(x,y) _STATIC_ASSERT_DO_JOIN(x,y)
    #define _STATIC_ASSERT_DO_JOIN(x,y) _STATIC_ASSERT_DO_JOIN2(x,y)
    #define _STATIC_ASSERT_DO_JOIN2(x,y) x##y

    #define static_assert(condition, message) \
    typedef ::vmbase::static_assert_helper<sizeof (::vmbase::STATIC_ASSERTION_FAILED<(bool)(condition)>)> \
        _STATIC_ASSERT_JOIN(MMgc_static_assert_line_, __LINE__)
#endif

#endif // __VmAssert__
