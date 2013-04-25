/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __AvmAssert__
#define __AvmAssert__

// AVMPLUS_CUSTOM_ASSERTION_HANDLER not yet (now == Apr-2009) exposed as a feature;
// awaiting cleanup of the debug code (merging MMgc and Avm debug macros, etc)

#if defined(AVMPLUS_CUSTOM_ASSERTION_HANDLER)
    extern "C"
    {
        void AVMPlusCustomAssertionHandler(const char *message);
    }
#endif

namespace avmplus
{
    void AvmDebugMsg(bool debuggerBreak, const char* format, ...);
    void AvmDebugMsg(const char* msg, bool debuggerBreak);
    void AvmDebugMsg(const wchar* msg, bool debuggerBreak);

#if defined DEBUG || defined _DEBUG

    inline void AvmAssertFail(const char *message) {
        #if defined(AVMPLUS_CUSTOM_ASSERTION_HANDLER)
            AVMPlusCustomAssertionHandler(message);
        #else
            AvmDebugMsg(message, true);
        #endif
    }

    inline void _AvmAssertMsg(int32_t condition, const char* message) {
        if (!condition)
            AvmAssertFail(message);
    }

    #define AvmAssert(condition) \
        _AvmAssert((condition), __LINE__,__FILE__)

    #define AvmAssertMsg(condition,message) \
        do { avmplus::_AvmAssertMsg((condition), (message)); } while (0) /* no semi */

    #define AvmAssertMsgCanThrow(condition,message,core) \
        do { TRY ((core), kCatchAction_ReportAsError) { \
                avmplus::_AvmAssertMsg((condition), (message)); \
             } CATCH(Exception* e) { avmplus::_AvmAssertMsg(0, "Exception caught in assert!\n"); (void)e; } \
             END_CATCH \
             END_TRY \
        } while (0) /* no semi */

    #define _AvmAssert(condition, line_, file_) \
        __AvmAssert((condition), line_, file_)

    #define __AvmAssert(condition, line_, file_) \
        do { AvmAssertMsg((condition), \
                          "Assertion failed: \"" #condition "\" (" #file_ ":" #line_ ")\n"); \
        } while (0) /* no semi */

#else

    #define AvmAssertMsgCanThrow(condition,message,core) \
        do { } while (0) /* no semi */

    #define AvmAssertMsg(condition,message) \
        do { } while (0) /* no semi */

    #define AvmAssert(condition) \
        do { } while (0) /* no semi */

#endif
}

#endif /* __AvmAssert__ */
