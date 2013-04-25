/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write C object from local into sparse Array";
include "driver.as"

class C
{
    var x: C = null;  // just some field to make the type nontrivial
}

function array_write_obj(iter: int): Array
{
    // It will become initialized after the first iteration but it's more honest
    // to initialize it here.
    var a: Array = new Array(10000);
    for ( var k:int = 0 ; k < 10000 ; k += 10 )
        a[k] = 0;

    // Please do not change the type annotations
    // Please do not inline the arithmetic into the index expressions here
    var v0: C = new C;
    var v1: C = new C;
    var v2: C = new C;
    var v3: C = new C;
    var v4: C = new C;
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

TEST(function () { array_write_obj(1000); }, "array-write-C-1");
