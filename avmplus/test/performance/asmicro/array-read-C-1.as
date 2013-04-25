/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read C object from dense Array, store in local with type *.";
include "driver.as"

class C
{
    var x: C = null;  // just some field to make the type nontrivial
}

function mingle(c1: C, c2: C, c3: C, c4: C, c5: C): C { return c3 }

function array_read_obj(iter: int, a:Array)
{
    // Please do not change the type annotation
    var tmp1:*;
    var tmp2:*;
    var tmp3:*;
    var tmp4:*;
    var tmp5:*;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp1 = a[j];  j++;
            tmp2 = a[j];  j++;
            tmp3 = a[j];  j++;
            tmp4 = a[j];  j++;
            tmp5 = a[j];  j++;
        }
    }
    return mingle(tmp1,tmp2,tmp3,tmp4,tmp5);
}

var ao: Array = new Array(1000);
(function () {
    var tmp = new C;
    for ( var i=0 ; i < ao.length ; i++ )
        ao[i] = tmp;
})();
TEST(function () { array_read_obj(1000, ao); }, "array-read-C-1");
