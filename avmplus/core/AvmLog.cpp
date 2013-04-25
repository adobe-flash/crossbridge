/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
    void AvmLog(const char* format, ...)
    {
        va_list args;
        va_start(args, format);

        char buf[2048];
        VMPI_vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

        VMPI_log(buf);
    }
}
