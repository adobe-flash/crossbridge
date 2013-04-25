/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "XXX";
// var VERSION = "AS3";
// var TITLE   = "delete operator";


var flt4:float4 = new float4(1,2,3,4);

// In-range deletion - these are not deletable so the result should be false and the element should be there still

Assert.expectEq("delete an in-range element of a float4", false, delete flt4[2]);
Assert.expectEq("delete an in-range element of a float4", false, delete flt4["2"]);
Assert.expectEq("deleted element is still there", 3, flt4[2]);

// Out-of-range deletion with an index property - we're working with the "Vector" nature of float4 so the result should be true

Assert.expectEq("delete an out-of-range element of a float4", true, delete flt4[4]);

// Out-of-range deletion with non-index properties - since it's a sealed object we should throw the customary exception for
// sealed object deletion attempts.

Assert.expectError("delete an out-of-range element of a float4 (negative)", "ReferenceError: Error #1120", function () { delete flt4[-1] });
Assert.expectError("delete an out-of-range element of a float4 (non-integer)", "ReferenceError: Error #1120", function () { delete flt4["4.75"] });
Assert.expectError("delete an out-of-range element of a float4 (non-number)", "ReferenceError: Error #1120", function () { delete flt4["zappa"] });

