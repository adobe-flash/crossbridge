/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCThreadLocal__
#define __GCThreadLocal__

namespace MMgc
{
    template<typename T>
    class GCThreadLocal
    {
    public:
        GCThreadLocal()
        : valid(false)
        {
            GCAssert(sizeof(T) <= sizeof(void*));
            bool r = VMPI_tlsCreate(&tlsId);
            GCAssert(r);
            (void)r;
            valid = true;
            VMPI_tlsSetValue(tlsId, NULL);
        }

        ~GCThreadLocal()
        {
            destroy();
        }

        void destroy()
        {
            if (valid) {
                valid = false;
                VMPI_tlsDestroy(tlsId);
            }
        }

        T operator=(T tNew)
        {
            VMPI_tlsSetValue(tlsId, (void*) tNew);
            return tNew;
        }
        operator T() const
        {
            return (T) VMPI_tlsGetValue(tlsId);
        }
        T operator->() const
        {
            return (T) VMPI_tlsGetValue(tlsId);
        }
    private:
        bool valid;         // Use a separate flag to avoid depending on any particular value of tlsId
        uintptr_t tlsId;
    };
}

#endif
