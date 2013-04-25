/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write int from local into sparse Array";
include "driver.as"

function array_write_int(iter: int): Array
{
    // It will become initialized after the first iteration but it's more honest
    // to initialize it here.
    var a: Array = new Array(10000);
    for ( var k:int = 0 ; k < 10000 ; k += 10 )
        a[k] = 0;

    // Please do not change the type annotations
    // Please do not inline the arithmetic into the index expressions here
    var v0: int = 1;
    var v1: int = 2;
    var v2: int = 3;
    var v3: int = 4;
    var v4: int = 5;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 10000 ; ) {
            a[j] = v0;  j += 10;
            a[j] = v1;  j += 10;
            a[j] = v2;  j += 10;
            a[j] = v3;  j += 10;
            a[j] = v4;  j += 10;
        }
    }
    if (a.length != 10000) throw "WROTE PAST END";
    return a;
}

TEST(function () { array_write_int(1000); }, "array-write-int-2");
