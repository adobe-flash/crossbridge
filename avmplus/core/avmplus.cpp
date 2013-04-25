/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


//
// This is the source file for the avmplus project's precompiled header.
//

#include "avmplus.h"

// The default precompiled header setting on project avmplus is
// to replace up to #include "avmplus.h" with avmplus.pch, the
// precompiled header.

// avmplus.cpp has custom precompiled header settings to create
// avmplus.pch by precompiling this file up until the
// #include "avmplus.h".

// #include "avmplus.h" should be the first non-comment line in
// any CPP in this project, as the default will be to substitute
// the contents of the pch for everything up to the include.

// If a file cannot #include "avmplus.h", or cannot include it
// first, then it should disable the precompiled header for that file only
// (for instance, pcre.cpp).

