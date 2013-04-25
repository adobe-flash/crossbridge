/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
Returns a float value with positive sign, greater than or equal to 0 but less
than 1, chosen randomly or pseudo randomly with approximately uniform distribution
over that range, using an implementation-dependent algorithm or strategy. This
function takes no arguments.
*/

// var SECTION = "4.5.28";
// var VERSION = "AS3";
// var TITLE   = "public function random():float";



AddStrictTestCase("float.random() returns a float", "float", getQualifiedClassName(float.random()));
AddStrictTestCase("float.random() length is 0", 0, float.random.length);

var myRandom:float;
for (var x:int = 0; x < 1000; x++)
{
    myRandom = float.random();
    if ( myRandom < 0f)
        Assert.expectEq("float.random() illegal value returned", "<0f", myRandom);
    if ( myRandom > 1f)
        Assert.expectEq("float.random() illegal value returned", ">1f", myRandom);
}



