/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __vmbase__
#define __vmbase__

#include "VMAssert.h"
#include "VMPI.h"

// vmbase.h is included by both avmplus.h and MMgc.h
// It serves as the main header for the vmbase module, which contains platform
// independent code used by both avmcore and MMgc, e.g. AvmAssert.

namespace vmbase
{
    class SafepointRecord;
}

#include "VMThread.h"
#include "Safepoint.h"
#include "vmbase-inlines.h"

#endif /* __vmbase__ */
