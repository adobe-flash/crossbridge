/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Test cases for the exact-gc tracer generator script.
//
// The file is compiled and compilation errors will be signaled for various kinds of
// problems.  Additionally, visual inspection of the generated tracing code is recommended.
// That is not ideal, but it's better than nothing.
//
// These tests are for corner cases / hard cases, not for mainstream cases - those
// are tested well enough by all the Tamarin and Flash Player code that's annotated

#ifndef __avmplus_exactgcTests__
#define __avmplus_exactgcTests__

namespace avmplus
{
#ifdef DEBUG
    // This class must never be instantiated.

    class GC_CPP_EXACT(ExactGCTest, ScriptObject)
    {
    public:
        ExactGCTest() : ScriptObject(NULL,NULL) {}

        GC_DATA_BEGIN(ExactGCTest)

        // Testing if, elif, else.  When compiling, we'll hit the third case, !CASE3NEG.
        // For more elaborate testing we could set up something that hits each of the arms
        // in separate compilations (or through multiple inclusions of this file with different
        // type names, maybe)

#if CASE1
        ScriptObject* GC_POINTER(case1_x);
        ScriptObject* GC_POINTER(case1_w);
        ScriptObject* GC_POINTER(case1_and_3_w);  // Reused below, too
#elif CASE2
        ScriptObject* GC_POINTER(case2_x);
        ScriptObject* GC_POINTER(case2_z);
#elif !CASE3NEG
        ScriptObject* GC_POINTER(case3_z);
        ScriptObject* GC_POINTER(case1_and_3_w);  // From above; we allow dupes in different arms of the switch
#else
        ScriptObject* GC_POINTER(else_r);
#endif
        //Test multiple member declaration
        GCMember<ScriptObject>  sobject1, sobject2 ,sobject3;
        //spaces between 'GCMember' and '<' should get handled correctly
        GCMember <ScriptObject> sobject4;
        //Nested Template with GCMember
        GCMember < ExactHeapList< GCList<ScriptObject> > > nestedTemplate;
        // GCMember inside a comment
        int*     ptr1;          //The script should not fail because of this GCMember<>
        int*     ptr2;          /* The script should not fail because of this GCMember<> */

        GC_DATA_END(ExactGCTest)

        // These reference all the variables above, if ifdefs aren't emitted
        // correctly we should get a compilation error.
        
#if CASE1
        void someFunction() {
            case1_x = NULL;
            case1_w = NULL;
            case1_and_3_w = NULL;
        }
#elif CASE2
        void someFunction() {
            case2_x = NULL;
            case2_z = NULL;
        }
#elif !CASE3NEG
        void someFunction() {
            case3_z = NULL;
            case1_and_3_w = NULL;
        }
#else
        void someFunction() {
            else_r = NULL;
        }
#endif

        void someFunction2() {
            someFunction();     // Make sure at least one of them is defined
        }
    };
#endif
}

#endif // __avmplus_exactgcTests__
