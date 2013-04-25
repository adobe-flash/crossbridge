/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Compute float4.magnitude3 of a large Vector.<float4>";
include "driver.as"

function float4_magnitude3(iter: int, a: Vector.<float4>): float4
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:float4;
    var tmp2:float4;
    var tmp3:float4;
    var tmp4:float4;
    var tmp5:float4;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp1 = a[j];  tmp1 = float4.magnitude3(tmp1); j++;
            tmp2 = a[j];  tmp2 = float4.magnitude3(tmp2); j++;
            tmp3 = a[j];  tmp3 = float4.magnitude3(tmp3); j++;
            tmp4 = a[j];  tmp4 = float4.magnitude3(tmp4); j++;
            tmp5 = a[j];  tmp5 = float4.magnitude3(tmp5); j++;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

function init() {
    var v:Vector.<float4> = new Vector.<float4>(1000)
    for (var i:uint = 0, len:uint = v.length; i < len; ++i)
        v[i] = float4(i,-i,i,-i)
    return v
}

var v: Vector.<float4> = init()

TEST(function () { float4_magnitude3(1000, v); }, "float4-magnitude3");
