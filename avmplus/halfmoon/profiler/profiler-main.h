/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// This is the main private header file for the profiler module.
// Main hooks are into NanoJIT
//
#ifndef PROFILER_MAIN_H_
#define PROFILER_MAIN_H_

// Include public header first
#include "profiler.h"

#ifdef VMCFG_HALFMOON

// Include internal headers next.
#include "profiler-types.h"
#include "profiler-methodprofile.h"  
#include "profiler-profiledstate.h"

#endif // VMCFG_HALFMOON

#endif // PROFILER_MAIN_H_
