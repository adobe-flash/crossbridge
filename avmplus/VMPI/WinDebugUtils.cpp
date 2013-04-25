/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"

#ifdef _DEBUG
    #include <windows.h>
    #include <malloc.h>
    #ifndef UNDER_CE
        #include <DbgHelp.h>
    #endif
    #include <strsafe.h>
#endif

/*************************************************************************/
/******************************* Debugging *******************************/
/*************************************************************************/

void VMPI_debugLog(const char* message)
{
#ifndef UNDER_CE
    OutputDebugStringA(message);
#else
    // !!@ only unicode is supported
    //OutputDebugStringW(unicode msg);
#endif

    VMPI_log( message ); //also log to standard output
}


void VMPI_debugBreak()
{
    ::DebugBreak();
}
