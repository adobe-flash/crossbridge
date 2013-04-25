/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


//
// This is the source file for the nanojit project's precompiled header.
//

#include "nanojit.h"

// The default precompiled header setting on project nanojit is
// to replace up to #include "nanojit.h" with nanojit.pch, the
// precompiled header.

// nanojit.cpp has custom precompiled header settings to create
// nanojit.pch by precompiling this file up until the
// #include "nanojit.h".

// #include "nanojit.h" should be the first non-comment line in
// any CPP in this project, as the default will be to substitute
// the contents of the pch for everything up to the include.

// If a file cannot #include "nanojit.h", or cannot include it
// first, then it should disable the precompiled header for that file only
// (for instance, pcre.cpp).

