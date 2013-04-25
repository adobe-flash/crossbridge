/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __nanojit_Fragmento__
#define __nanojit_Fragmento__

namespace nanojit
{
    struct GuardRecord;

    /**
     * Fragments are linear sequences of native code that have a single entry
     * point at the start of the fragment and may have one or more exit points
     *
     * It may turn out that that this arrangement causes too much traffic
     * between d and i-caches and that we need to carve up the structure differently.
     */
    class Fragment
    {
        public:
            Fragment(const void*
                     verbose_only(, uint32_t profFragID));
//### FIXME
#ifdef NANOJIT_THUMB2
            NIns*           code()                          { return (NIns*)((uintptr_t)_code | 0x1); }
#else
            NIns*           code()                          { return _code; }
#endif
            void            setCode(NIns* codee)            { _code = codee; }
            int32_t&        hits()                          { return _hits; }

            LirBuffer*     lirbuf;
            LIns*          lastIns;

            const void* ip;
            uint32_t recordAttempts;
            NIns* fragEntry;

            // for fragment entry and exit profiling.  See detailed
            // how-to-use comment below.
            verbose_only( LIns*          loopLabel; ) // where's the loop top?
            verbose_only( uint32_t       profFragID; )
            verbose_only( uint32_t       profCount; )
            verbose_only( uint32_t       nStaticExits; )
            verbose_only( size_t         nCodeBytes; )
            verbose_only( size_t         nExitBytes; )
            verbose_only( uint32_t       guardNumberer; )
            verbose_only( GuardRecord*   guardsForFrag; )

        private:
            NIns*            _code;        // ptr to start of code
            int32_t          _hits;
    };
}

/*
 * How to use fragment profiling
 *
 * Fragprofiling adds code to count how many times each fragment is
 * entered, and how many times each guard (exit) is taken.  Using this
 * it's possible to easily find which fragments are hot, which ones
 * typically exit early, etc.  The fragprofiler also gathers some
 * simple static info: for each fragment, the number of code bytes,
 * number of exit-block bytes, and number of guards (exits).
 *
 * Fragments and guards are given unique IDs (FragID, GuardID) which
 * are shown in debug printouts, so as to facilitate navigating from
 * the accumulated statistics to the associated bits of code.
 * GuardIDs are issued automatically, but FragIDs you must supply when
 * calling Fragment::Fragment.  Supply values >= 1, and supply a
 * different value for each new fragment (doesn't matter what, they
 * just have to be unique and >= 1); else
 * js_FragProfiling_FragFinalizer will assert.
 *
 * How to use/embed:
 *
 * - use a debug build (one with NJ_VERBOSE).  Without it, none of
 *   this code is compiled in.
 *
 * - set LC_FragProfile in the lcbits of the LogControl* object handed
 *   to Nanojit
 *
 * When enabled, Fragment::profCount is incremented every time the
 * fragment is entered, and GuardRecord::profCount is incremented
 * every time that guard exits.  However, NJ has no way to know where
 * the fragment entry/loopback point is.  So you must set
 * Fragment::loopLabel before running the assembler, so as to indicate
 * where the fragment-entry counter increment should be placed.  If
 * the fragment does not naturally have a loop label then you will
 * need to artificially add one.
 *
 * It is the embedder's problem to fish out, collate and present the
 * accumulated stats at the end of the Fragment's lifetime.  A
 * Fragment contains stats indicating its entry count and static code
 * sizes.  It also has a ::guardsForFrag field, which is a linked list
 * of GuardRecords, and by traversing them you can get hold of the
 * exit counts.
 */

#endif // __nanojit_Fragmento__
