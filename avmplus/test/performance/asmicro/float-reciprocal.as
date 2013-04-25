/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Compute float.reciprocal of a large Vector.<float>";
include "driver.as"

function float_reciprocal(iter: int, a: Vector.<float>): float
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:float;
    var tmp2:float;
    var tmp3:float;
    var tmp4:float;
    var tmp5:float;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp1 = a[j];  tmp1 = float.reciprocal(tmp1); j++;
            tmp2 = a[j];  tmp2 = float.reciprocal(tmp2); j++;
            tmp3 = a[j];  tmp3 = float.reciprocal(tmp3); j++;
            tmp4 = a[j];  tmp4 = float.reciprocal(tmp4); j++;
            tmp5 = a[j];  tmp5 = float.reciprocal(tmp5); j++;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

function init() {
    var v:Vector.<float> = new Vector.<float>(1000)
    for (var i:uint = 0, len:uint = v.length; i < len; ++i)
        v[i] = float((i & 1) ? i : -i);
    return v
}

var v: Vector.<float> = init()

TEST(function () { float_reciprocal(1000, v); }, "float-reciprocal");
