/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"

// Helper functions for VMPI_callWithRegistersSaved, kept in this file to prevent them from
// being inlined in MMgcPortUnix.cpp / MMgcPortMac.cpp.

// Registers have been flushed; compute a stack pointer and call the user function.
void CallWithRegistersSaved2(void (*fn)(void* stackPointer, void* arg), void* arg, void* buf)
{
    (void)buf;
    volatile int temp = 0;
    fn((void*)((uintptr_t)&temp & ~7), arg);
}

// Do nothing - just called to prevent another call from being a tail call, and to keep some values alive
void CallWithRegistersSaved3(void (*fn)(void* stackPointer, void* arg), void* arg, void* buf)
{
    (void)buf;
    (void)fn;
    (void)arg;
}

typedef void (*LoggingFunction)(const char*);

LoggingFunction logFunc = NULL;

void RedirectLogOutput(LoggingFunction func)
{
    logFunc = func;
}

LoggingFunction GetCurrentLogFunction()
{
    return logFunc;
}

void VMPI_TimerData::init(uint32_t micros, VMPI_TimerClient* client) {
    this->interval = micros;
    this->client = client;
}
