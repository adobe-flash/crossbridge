/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_AvmProfiler__
#define __avmplus_AvmProfiler__


namespace avmplus
{
#ifdef DEBUGGER
    /**
     * The Profiler class used to contain a profiler that is now no
     * longer in use. However, it still contains AS3 function showRedrawRegions,
     * and so it cannot be deleted.
     */
    class Profiler : public MMgc::GCFinalizedObject
    {
    public:
        Profiler()
        {
        }
    };
#endif
}

#endif /* __avmplus_AvmProfiler__ */
