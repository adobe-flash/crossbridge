/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "bz bug 708996";


class Float4Matrix
{
    // public float4 access is the fastest way to get to our rows
    public var row0 : float4;
    public var row1 : float4;
    public var row2 : float4;
    public var row3 : float4;

    public function Float4Matrix ( ) {
        identity();
    }

    public function scale ( x:float, y:float, z:float ):void {
        row0 = float4(x,0.0f,0.0f,0.0f);
        row1 = float4(0.0f,y,0.0f,0.0f);
        row2 = float4(0.0f,0.0f,z,0.0f);
        row3 = float4(0.0f,0.0f,0.0f,1.0f);
    }

    public function identity():void {
        row0 = float4(1.0f,0.0f,0.0f,0.0f);
        row1 = float4(0.0f,1.0f,0.0f,0.0f);
        row2 = float4(0.0f,0.0f,1.0f,0.0f);
        row3 = float4(0.0f,0.0f,0.0f,1.0f);
    }
}

var f4m:Float4Matrix = new Float4Matrix();
f4m.scale(2,3,.4);

AddStrictTestCase("fm4.row0", float4(2,0,0,0), f4m.row0);
AddStrictTestCase("fm4.row1", float4(0,3,0,0), f4m.row1);
AddStrictTestCase("fm4.row2", float4(0,0,0.4000000059604645,0), f4m.row2);
AddStrictTestCase("fm4.row3", float4(0,0,0,1), f4m.row3);

