/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write int into Vector<int> with const+var expression.  Compare vector-write-int-1.";
include "driver.as"

function vector_write_int(iter: int)
{
    // Please do not change or remove the type annotations.
    // Please do not change the indexing expressions or updates.
    var a: Vector.<int> = new Vector.<int>(1000);
    var v0: int = 1;
    var v1: int = 2;
    var v2: int = 3;
    var v3: int = 4;
    var v4: int = 5;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; j += 5) {
            a[0+j] = v0;
            a[1+j] = v1;
            a[2+j] = v2;
            a[3+j] = v3;
            a[4+j] = v4;
        }
    }
}

TEST(function () { vector_write_int(1000); }, "vector-write-int-4");
