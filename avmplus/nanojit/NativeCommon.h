/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nanojit_NativeCommon__
#define __nanojit_NativeCommon__

namespace nanojit
{
    // In debug builds, Register is defined as a non-integer type to avoid
    // possible mix-ups with RegisterMask and integer offsets.  In non-debug
    // builds, it's defined as an integer just in case some compilers fail to
    // optimize single-element structs in the obvious way.
    //
    // Note that in either case, a Register can be initialized like so:
    //
    //   Register r = { 0 };
    //
    // In the debug case it's a struct initializer, in the non-debug case it's
    // a scalar initializer.
    //
    // XXX: The exception to all the above is that if NJ_USE_UINT32_REGISTER
    // is defined, the back-end is responsible for defining its own Register
    // type, which will probably be an enum.  This is just to give all the
    // back-ends a chance to transition smoothly.
#if defined(NJ_USE_UINT32_REGISTER)
    #define REGNUM(r) (r)

#elif defined(DEBUG) || defined(__SUNPRO_CC)
    // Always use struct declaration for 'Register' with
    // Solaris Studio C++ compiler, because it has a bug:
    // Scalar type can not be initialized by '{1}'.
    // See bug 603560.

    struct Register {
        uint32_t n;     // the register number
    };

    static inline uint32_t REGNUM(Register r) {
        return r.n;
    }

    static inline Register operator+(Register r, int c)
    {
        r.n += c;
        return r;
    }

    static inline bool operator==(Register r1, Register r2)
    {
        return r1.n == r2.n;
    }

    static inline bool operator!=(Register r1, Register r2)
    {
        return r1.n != r2.n;
    }

    static inline bool operator<=(Register r1, Register r2)
    {
        return r1.n <= r2.n;
    }

    static inline bool operator<(Register r1, Register r2)
    {
        return r1.n < r2.n;
    }

    static inline bool operator>=(Register r1, Register r2)
    {
        return r1.n >= r2.n;
    }

    static inline bool operator>(Register r1, Register r2)
    {
        return r1.n > r2.n;
    }
#else
    typedef uint32_t Register;

    static inline uint32_t REGNUM(Register r) {
        return r;
    }
#endif

#ifndef RA_REGISTERS_OVERLAP
    #define rmask(r) ( (RegisterMask(1)) << REGNUM(r) )
#endif  // otherwishe, it's the platform's responsibility to define rmask

} // namespace nanojit

#endif // __nanojit_NativeCommon__
