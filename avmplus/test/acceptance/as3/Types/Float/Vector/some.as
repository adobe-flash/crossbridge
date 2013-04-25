/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.some with float";


function ninetyNinePointNineNine_flt(value:float,index,obj) {
    if (value == float(99.99)) { // todo: use float literal
        return true;
    }
    return false;
}


var v2f = Vector.<float>([1.1, 3.1415, 33.33333, 99.99, 100.0000001]);
var v3f = Vector.<float>([1.1, 3.1415, 33.33333, 99.9901, 100.0000001]);

Assert.expectEq("Verify a value is in a float vector",
            true,
            v2f.some(ninetyNinePointNineNine_flt));

Assert.expectEq("Verify a value is not in a float vector",
            false,
            v3f.some(ninetyNinePointNineNine_flt));

