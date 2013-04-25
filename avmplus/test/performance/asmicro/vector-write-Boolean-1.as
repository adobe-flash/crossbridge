/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write Boolean into Vector<Boolean> with int index";
include "driver.as"

function vector_write_Boolean(iter: int): Vector.<Boolean>
{
    // Please do not change or remove the type annotations.
    // Please do not change the indexing expressions or updates.
    var a: Vector.<Boolean> = new Vector.<Boolean>(1000);
    var v0: Boolean = true;
    var v1: Boolean = false;
    var v2: Boolean = true;
    var v3: Boolean = false;
    var v4: Boolean = true;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            a[j] = v0;  j++;
            a[j] = v1;  j++;
            a[j] = v2;  j++;
            a[j] = v3;  j++;
            a[j] = v4;  j++;
        }
    }
    return a;
}

TEST(function () { vector_write_Boolean(1000); }, "vector-write-Boolean-1");
