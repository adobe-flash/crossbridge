/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array initializer on int values, compare with vector-init-2.";
include "driver.as"

function array_init_int(): *
{
    var t: Array;
    for ( var i:int=0 ; i < 10000 ; i++ )
        t = [1, 2, 3];
    return t;
}

TEST(function () { array_init_int(); }, "array-init-2");
