/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_atom__
#define __avmplus_atom__

namespace avmplus
{
    // Atom should really be an intptr_t, but doing so can cause problematic compiles
    // because some platforms define intptr_t as an int64_t, and some as a long, which
    // create different overload possibilities in a few cases. Ideally, Atom should
    // be a unique pointer type (as it is in TT) but for now, avoid the code churn
    // by defining it the "old" way
    //
    // math friendly pointer (64 bits in LP 64 systems)
    #if defined (_MSC_VER) && (_MSC_VER >= 1300)
        #define AVMPLUS_TYPE_IS_POINTER_SIZED __w64
    #else
        #define AVMPLUS_TYPE_IS_POINTER_SIZED
    #endif
    #ifdef AVMPLUS_64BIT
    typedef AVMPLUS_TYPE_IS_POINTER_SIZED int64_t Atom;
    #else
    typedef AVMPLUS_TYPE_IS_POINTER_SIZED int32_t Atom;
    #endif

    /**
     * The AtomConstants namespace defines constants for
     * manipulating atoms.
     *
     * The atom is a primitive value in ActionScript.  Since
     * ActionScript is a dynamically typed language, an atom can
     * belong to one of several types: null, undefined, number,
     * integer, string, boolean, object reference.
     *
     * Atoms are encoded with care to take up the minimum
     * possible space.  An atom is represented by a 32-bit
     * integer, with the bottom 3 bits indicating the type.
     *
     *      32 bit atom
     *
     *  31             16 15     8 7   3 210
     *  dddddddd dddddddd dddddddd ddddd TTT
     *
     *  TTT
     *  000  - untagged
     *  001  object
     *  010  string
     *  011  namespace
     *  100  undefined
     *  101  boolean
     *  110  integer
     *  111  double
     *
     *  - using last 3 bits means allocations must be 8-byte aligned.
     *  - related types are 1 bit apart, e.g. int/double
     *
     *  kIntptrType atoms are used to represent integer values from -2^28..2^28-1,
     *  regardless of whether the context implies int, uint, or Number.
     *  If a number doesn't fit into that range it is stored as a kDoubleType
     *
     */
    namespace AtomConstants
    {
        /**
         * @name Atom types
         * These are the type values that appear in the bottom
         * 3 bits of an atom.
         */
        /*@{*/
        // cannot use 0 as tag, breaks atomWriteBarrier
        const Atom kUnusedAtomTag    = 0;
        const Atom kObjectType       = 1;  // null=1
        const Atom kStringType       = 2;  // null=2
        const Atom kNamespaceType    = 3;  // null=3
        const Atom kSpecialBibopType = 4;  // undefined=4, payload=bibopPointer
        const Atom kBooleanType      = 5;  // false=5 true=13
        const Atom kIntptrType       = 6;
        const Atom kDoubleType       = 7;
        /*@}*/

        const uint8_t kBibopUndefined   = 0;
        const uint8_t kBibopFloatType   = 1;
        const uint8_t kBibopFloat4Type  = 2;

        /*
        other things you can do with math on atoms

        isNull          (unsigned)a < 4
        isUndefined     a == undefinedAtom
        isSpecial       (unsigned)a <= 4
        isNumber        a & 6 == 6 ( Note: this tests for "int | Number", but not float! ) 

                            ^8  jlt(a<8)    ^2      jle(a<=4)
        true        1110  0110  t           1100    f
        false       0110  1110  f           0100    t
        undefined   0100  1100  f           0110    f
        */

        /**
         * @name Special atom values
         * These are the atoms for undefined, null, true
         * and false.
         */
        /*@{*/
        // there is no single null atom.  use isNull() to test, or
        // one of the following typed null atoms.  if you must, use NULL (0).
        const Atom nullObjectAtom = kObjectType|0;
        const Atom nullStringAtom = kStringType|0;
        const Atom nullNsAtom     = kNamespaceType|0;
        const Atom undefinedAtom  = kSpecialBibopType|0; // 0x03
        const Atom trueAtom       = kBooleanType|0x08; // 0x0D
        const Atom falseAtom      = kBooleanType|0x00; // 0x05
        const Atom zeroIntAtom    = Atom(kIntptrType|0);

        // used in unreachable "return" clauses as a self-documenting invalid value.
        const Atom unreachableAtom = kUnusedAtomTag;
        /*@}*/

        const static uint32_t kBibopBlockSize = 4096;         // This must be the same as GCHeap::kBlockSize, we check that in atom.cpp

        /**
         * @name Useful constants for manipulating atoms
         * These are particularly useful when generating code in the JIT.
         */
        /*@{*/
        const Atom kAtomTypeMask = 7;       // mask for type tag
        const Atom kAtomPtrMask = ~7;       // mask for atom pointer, or shifted Intptr value
        const unsigned kAtomTypeSize = 3;   // width of type tag
        const Atom kBibopTypePtrMask = ~(Atom)(kBibopBlockSize - 1); // mask for bibop type pointer (char*, first char from the bibop page)
        /*@}*/

        // This is a special value used internally by some VM code to mean
        // "no such value" in situations where "undefinedAtom" can be a valid
        // value; it is technically not a legal Atom and can only be
        // used in some well-defined cases. It should *never* be used outside of the VM;
        // the VM will *never* return this value to calling code; the VM *may or may not*
        // accept this value as a valid Atom if it is passed in from calling code. Caveat emptor.
        const Atom atomNotFound = Atom(0);
    }

    // atomMinIntValue is the smallest negative (most negative) integer value representable
    // in a native word, and atomMaxIntValue is the largest positive integer value thus
    // representable, in both cases constrained such that interchanging the value to and
    // from a 'double' can be done without loss of precision.
    //
    //  - On 32-bit systems an intptr_t is 32 bits wide so the range is -2^28 .. 2^28-1 (29 bit range).
    //
    //  - On 64-bit systems an intptr_t is 64 bits wide so the value is actually constrained by
    //    the available bits in a double.  There are 52 bits of unsigned mantissa represented explicitly,
    //    an implicit leading 1 bit, and a sign bit.  This gives a total of 54 bits in a signed-magnitude
    //    representation, which would ordinarily allow a range of -2^53-1 .. 2^53-1.  Due to the implicit
    //    leading 1 bit, however, larger powers of 2 can be represented with an appropriate exponent.
    //    As a result, the contiguous range of exactly-representable integers is actually extended to
    //    -2^53 .. 2^53.  When working with integer values in intptr_t form, however, we use a twos-complement
    //    representation, in which 54 bits provides a range of -2^53 .. 2^53-1.  We represent all integers
    //    outside this range as doubles.  Being slightly conservative in this way allows us to verify that
    //    an integer value is in range merely by verifying that it fits in 54 bits in the native twos-complement
    //    form, without explicit comparison with the bounds.  These bounds should be used consistently, so that
    //    all integer values have a canonical representation as atoms. NOTE: There is no absolute requirement
    //    that integers be in canonical form, and correctness must never depend on it.  For performance
    //    reasons, we sometimes roll over to the double-float  representation at 32 bits in order to exploit a
    //    cheaper test.  See https://bugzilla.mozilla.org/show_bug.cgi?id=601426#c6 .
    //
    // atomSignExtendShift is the number of bits to shift an untagged intptr_t left and
    // then right in order to properly sign extend it.  (I don't like the 'atom' prefix but
    // it fits in with the pattern of the others.)

#ifdef AVMPLUS_64BIT
    const intptr_t atomMinIntValue = -(1LL<<53);
    const intptr_t atomMaxIntValue = (1LL<<53)-1;
    const intptr_t atomSignExtendShift = 10;         // 54 significant bits
#else
    const intptr_t atomMinIntValue = -(1L<<28);
    const intptr_t atomMaxIntValue = (1L<<28)-1;
    const intptr_t atomSignExtendShift = 3;          // 29 signficant bits
#endif

    // sadly, these generate better code than the inlines in atom-inlines.h
    #define atomKind(a)     ((avmplus::Atom)(uintptr_t(a) & 7))
    #define atomPtr(a)      ((void*)(uintptr_t(a) & ~7))
    // bibopKind should be kBibopFloatType. 
    // In the future also kBibopFloat4Type, kBibopDecimalType etc.
    // In debug builds bibopKind() is a more elaborate inline function in atom-inlines.h.
#ifndef DEBUG
    #define bibopKind(a)     (*(uint8_t*)(uintptr_t(a) & kBibopTypePtrMask))
#endif
    
    #define ISNULL(a) (((uintptr_t)a) < (uintptr_t)kSpecialBibopType)

    // returns true if atom type is int.
    // Note that this DOES NOT imply that the Atom's value will
    // fit into an int32_t (or a uint32_t); an integer atom's value
    // will always fit into an intptr_t, which may be more than
    // an int32_t (or uint32_t) will hold on 64-bit architectures.
    bool            atomIsIntptr(Atom atom);

    // return true iff both atoms are of kIntptrType. sometimes this is slightly
    // faster than calling atomIs(b1)&&atomIs(b2). (And yes, "atomIsBoth" is poor
    // grammar in English, but follows the "atomIsXXX -> bool" pattern)
    bool            atomIsBothIntptr(Atom atom1, Atom atom2);

    // if the atom is kIntptrType and will fit into an int32_t, return true
    // if the atom is kIntptrType and will NOT fit into an int32_t, return false
    // if the atom is not kIntptrType, assert
    bool            atomCanBeInt32(Atom atom);
    // if the atom is kIntptrType and will fit into an uint32_t, return true
    // if the atom is kIntptrType and will NOT fit into an uint32_t, return false
    // if the atom is not kIntptrType, assert
    bool            atomCanBeUint32(Atom atom);

    // getters to obtain the value.
    // these getters require that the type of the Atom be *exactly* what is requested;
    // no coercion is done (e.g., calling getDouble() on a Atom containing TypeInt will assert).
    intptr_t        atomGetIntptr(Atom atom);

    // if the given intptr can fit into an kIntptrType atom, return true.
    // if not (the magnitude is too large/small), return false.
    // WARNING: do not pass an (unsigned) uintptr_t value; you may get incorrect results.
    bool            atomIsValidIntptrValue(const intptr_t i);

    // if the given uintptr_t can fit into an kIntptrType atom, return true.
    // if not (the magnitude is too large/small), return false.
    // WARNING: do not pass a (signed) intptr_t value; you may get incorrect results.
    bool            atomIsValidIntptrValue_u(const uintptr_t u);

    // make an int atom from a signed value known to fit
    Atom atomFromIntptrValue(const intptr_t i);

    // make an int atom from an unsigned value known to fit
    Atom atomFromIntptrValue_u(const uintptr_t u);

    // macro for profiling the frequencies of different Atom tags.
    // Must include vprof.h.  usage:   tagprof("histogram for my_atom", my_atom);
    #ifdef DOPROF
    #  define tagprof(n,v) _nhprof(n, uintptr_t(v)&7, 8, 0, 1, 2, 3, 4, 5, 6, 7)
    #else
    #  define tagprof(n,v)
    #endif

    typedef struct _Atom* OpaqueAtom;  // so we dont collide with int32_t
}

#endif // __avmplus_atom__
