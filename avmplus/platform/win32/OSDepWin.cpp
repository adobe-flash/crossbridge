/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include <windows.h>
#include <wctype.h>
#include <time.h>

#pragma warning(disable:4201)   // nonstandard extension used : nameless struct/union
#include <mmsystem.h>           // for timeGetTime (oddly, this include must come AFTER avmplus.h)

namespace avmplus
{
    void CALLBACK intWriteTimerProc(UINT, UINT, DWORD_PTR dwUser, DWORD_PTR, DWORD_PTR)
    {
        volatile int *i = (volatile int*)dwUser;
        *i = 1;
    }

    // The constant TIME_KILL_SYNCHRONOUS is only available if WINVER >= 0x0501 (== WinXP)
    static const UINT kTimeKillSynchronous = 0x0100;

    uintptr_t OSDep::startIntWriteTimer(uint32_t millis, volatile int *addr)
    {
        return (uintptr_t) timeSetEvent(millis, millis, (LPTIMECALLBACK)intWriteTimerProc, (DWORD_PTR)addr,
            TIME_PERIODIC | TIME_CALLBACK_FUNCTION
#ifndef UNDER_CE
            | kTimeKillSynchronous
#endif
            );
    }

    void OSDep::stopTimer(uintptr_t handle)
    {
        timeKillEvent((UINT)handle);
    }
}
