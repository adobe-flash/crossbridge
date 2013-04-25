/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.sort(comparefn) with float";


class TestClass {
    private var myVal:Object;
    public function TestClass(v:Object):void {
        myVal = v;
    }
    public function toString():String {
        return myVal.toString();
    }
    public function get val():Object {
        return myVal;
    }
}

var testClassVector = new Vector.<TestClass>();
for (var i=0; i<20; i++) {
    testClassVector.push(new TestClass(i));
}
// push one duplicate value
testClassVector.push(new TestClass(12));

// regular sort returning non-standard values
var myFloatSortFunction:Function = function (x,y):float {
    if (x.val < y.val)
        return float(-Infinity);  // TODO: a float literal for "-Infinity"?
    if (x.val > y.val)
        return float.MAX_VALUE;
    return float.NaN;
}

Assert.expectEq("Custom vector sort using sort function with non-standard (float) values",
            "0,1,2,3,4,5,6,7,8,9,10,11,12,12,13,14,15,16,17,18,19",
            testClassVector.sort(myFloatSortFunction).toString()
            );

