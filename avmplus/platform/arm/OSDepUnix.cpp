/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <stdlib.h>
#include <sys/time.h>

#include "avmplus.h"

namespace avmplus
{
    uint64_t OSDep::currentTimeMillis()
    {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        uint64_t result = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
        return result;
    }
}
