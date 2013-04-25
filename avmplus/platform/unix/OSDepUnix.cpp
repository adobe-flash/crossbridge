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

#ifdef DEBUGGER
    struct IntWriteTimerData
    {
        uint32_t interval; // in microseconds
        pthread_t thread;
        volatile int *addr;
    };

    void *timerThread(void *arg)
    {
        IntWriteTimerData *data = (IntWriteTimerData*)arg;
        volatile int *addr = data->addr;
        uint32_t interval = data->interval;
        while(data->addr)
        {
            usleep(interval);
            *addr = 1;
        }
        pthread_exit(NULL);
        return NULL;
    }

    uintptr_t OSDep::startIntWriteTimer(uint32_t millis, volatile int *addr)
    {
        pthread_t p;
        IntWriteTimerData *data = mmfx_new( IntWriteTimerData() );
        data->interval = millis*1000;
        data->addr = addr;
        pthread_create(&p, NULL, timerThread, data);
        data->thread = p;
        return (uintptr_t)data;
    }

    void OSDep::stopTimer(uintptr_t handle)
    {
        IntWriteTimerData *data = (IntWriteTimerData*) handle;
        data->addr = NULL;
        pthread_join(data->thread, NULL);
        mmfx_delete( data );
    }
#endif // DEBUGGER
}
