/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_OSDep__
#define __avmplus_OSDep__


namespace avmplus
{
    /**
     * The OSDep class is used to provide an abstraction layer
     * over OS-dependent code required by the VM.  This is
     * typically code that accesses system services, such
     * as date/time.
     */
    class OSDep
    {
    public:
        static uintptr_t startIntWriteTimer(uint32_t millis, volatile int *addr);
        static void stopTimer(uintptr_t handle);
    };
}

#endif /* __avmplus_OSDep__ */
