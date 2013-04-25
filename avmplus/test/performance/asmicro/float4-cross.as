/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Compute float4.cross of a large Vector.<float4>";
include "driver.as"

function float4_cross(iter: int, a: Vector.<float4>): float4
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:float4;
    var tmp2:float4;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        tmp1 = a[0]
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp2 = a[j]; tmp1 = float4.cross(tmp1, tmp2); j++;
            tmp2 = a[j]; tmp1 = float4.cross(tmp1, tmp2); j++;
            tmp2 = a[j]; tmp1 = float4.cross(tmp1, tmp2); j++;
            tmp2 = a[j]; tmp1 = float4.cross(tmp1, tmp2); j++;
            tmp2 = a[j]; tmp1 = float4.cross(tmp1, tmp2); j++;
        }
    }
    return tmp1
}

function init() {
    var v:Vector.<float4> = new Vector.<float4>(1000)
    for (var i:uint = 0, len:uint = v.length; i < len; ++i)
        v[i] = float4(i,-i,i,-i)
    return v
}

var v: Vector.<float4> = init()

TEST(function () { float4_cross(1000, v); }, "float4-cross");
