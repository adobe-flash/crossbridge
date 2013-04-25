/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read object of type C from Vector<C> with uint index, store in local with type C.";
include "driver.as"

class C
{
    var x: C = null;  // just some field to make the type nontrivial
}

function mingle(c1: C, c2: C, c3: C, c4: C, c5: C): C { return c3 }

function vector_read_obj(iter: uint, a: Vector.<C>): C
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:C;
    var tmp2:C;
    var tmp3:C;
    var tmp4:C;
    var tmp5:C;
    for ( var i:uint = 0 ; i < iter ; i++ ) {
        for ( var j:uint = 0 ; j < 1000 ; ) {
            tmp1 = a[j];  j++;
            tmp2 = a[j];  j++;
            tmp3 = a[j];  j++;
            tmp4 = a[j];  j++;
            tmp5 = a[j];  j++;
        }
    }
    return mingle(tmp1,tmp2,tmp3,tmp4,tmp5);
}

var vo: Vector.<C> = new Vector.<C>(1000);
(function () {
    var tmp = new C;
    for ( var i=0 ; i < vo.length ; i++ )
        vo[i] = tmp;
})();
TEST(function () { vector_read_obj(1000, vo); }, "vector-read-C-2");
