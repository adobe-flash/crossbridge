/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Origin: Bugzilla 599923, fixed by Bugzilla 678952.
var DESC = "Call charCodeAt on value read from Vector.<String> with int index and explicit temp, store in local with type Number, compare vector-read-char-1.";
include "driver.as"

function vector_read_char(iter: int, a: Vector.<String>, c:int): Number
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:Number;
    var tmp2:Number;
    var tmp3:Number;
    var tmp4:Number;
    var tmp5:Number;
    var tmp :String;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp = a[j];  tmp1=tmp.charCodeAt(c);  j++;
            tmp = a[j];  tmp2=tmp.charCodeAt(c);  j++;
            tmp = a[j];  tmp3=tmp.charCodeAt(c);  j++;
            tmp = a[j];  tmp4=tmp.charCodeAt(c);  j++;
            tmp = a[j];  tmp5=tmp.charCodeAt(c);  j++;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var vs: Vector.<String> = new Vector.<String>(1000);
(function () {
    for ( var i=0 ; i < 1000 ; i++ )
        vs[i] = String.fromCharCode(97+(i%26));
})();
TEST(function () { vector_read_char(1000, vs, 0); }, "vector-read-char-2");
