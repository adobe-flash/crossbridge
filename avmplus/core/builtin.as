/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// each class is in its own file, we include them all here
// so they end up in a single script that initializes all
// at once, in the order of includes below.

include "Object.as"
include "Class.as"
include "Function.as"
include "Namespace.as"
include "Boolean.as"
include "Number.as"
include "Float.as"
include "String.as"
include "Array.as"
include "actionscript.lang.as"
include "Vector.as"
include "DescribeType.as"
