/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nanojit.h"

namespace nanojit
{
    #ifdef FEATURE_NANOJIT

    using namespace avmplus;

    //
    // Fragment
    //
    Fragment::Fragment(const void* _ip
                       verbose_only(, uint32_t profFragID))
        :
          lirbuf(NULL),
          lastIns(NULL),
          ip(_ip),
          recordAttempts(0),
          fragEntry(NULL),
          verbose_only( loopLabel(NULL), )
          verbose_only( profFragID(profFragID), )
          verbose_only( profCount(0), )
          verbose_only( nStaticExits(0), )
          verbose_only( nCodeBytes(0), )
          verbose_only( nExitBytes(0), )
          verbose_only( guardNumberer(1), )
          verbose_only( guardsForFrag(NULL), )
          _code(NULL),
          _hits(0)
    {
        // when frag profiling is enabled, profFragID should be >= 1,
        // else it should be zero.  However, there's no way to assert
        // that here since there's no way to determine whether frag
        // profiling is enabled.
    }
    #endif /* FEATURE_NANOJIT */
}


