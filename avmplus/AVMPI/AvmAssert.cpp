/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "AVMPI.h"

namespace avmplus
{
    void AvmDebugMsg(bool debuggerBreak, const char* format, ...)
    {
    #ifdef _DEBUG
        char buf[1024];
        va_list args;
        va_start(args, format);

        VMPI_vsnprintf(buf, sizeof(buf), format, args);
        // Belt and suspenders, remove when we're sure VMPI_vsnprintf is completely reliable
        buf[sizeof(buf)-2] = '\n';
        buf[sizeof(buf)-1] = '\0';
        va_end(args);
        AvmDebugMsg(buf, debuggerBreak);
    #else
        (void)debuggerBreak;
        (void)format;
    #endif
    }

    void AvmDebugMsg(const char* p, bool debugBreak)
    {
    #ifdef _DEBUG
        VMPI_debugLog(p);
        if(debugBreak)
            VMPI_debugBreak();
    #else
        (void)p;
        (void)debugBreak;
    #endif
    }
}
