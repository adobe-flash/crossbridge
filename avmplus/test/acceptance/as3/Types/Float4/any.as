/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "4.6.4";
// var VERSION = "AS3";
// var TITLE   = "public function get any(): Boolean";

/*
Let v be this float4 value. Return true if ToBoolean(v:x) is true or ToBoolean(v:y)
is true or ToBoolean(v:z) is true or ToBoolean(v:w) is true. Otherwise return false
 */


var flt4:float4 = new float4(1f);
AddStrictTestCase("flt4.any returns a Boolean", "Boolean", getQualifiedClassName(flt4.any));

function check(param:float4):Boolean { return param.any; }

var x:int = 0;
var y:int = 0;
var z:int = 0;
var w:int = 0;
var expect:Boolean = false;
for ( x = 0; x <=1; x++ )
{
    for ( y = 0; y <=1; y++ )
    {
        for ( z = 0; z <=1; z++ )
        {
            for ( w = 0; w <=1; w++ )
            {
                flt4 = new float4(x, y, z, w);
                AddStrictTestCase("flt4.any float4("+x+", "+y+", "+z+", "+w +")", expect, flt4.any);
                AddStrictTestCase("flt4.any float4("+x+", "+y+", "+z+", "+w +") check()", expect, check(flt4));
                // Only the first iteration should produce a false value, all others will be true
                if (!expect) expect = true;
            }
        }
    }
}


