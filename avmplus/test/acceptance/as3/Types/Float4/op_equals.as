/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.6.1";
// var VERSION = "AS3";
// var TITLE   = "The == operator augmented by float4 values";


function check(param1:*, param2:*):* { return param1 == param2; }

Assert.expectEq("float4(1f) == float4(1f)", true, float4(1f) == float4(1f));
Assert.expectEq("float4(0f) == float4(0f)", true, float4(0f) == float4(0f));
Assert.expectEq("float4(0f) == float4(-0f)", true, float4(0f) == float4(-0f));
Assert.expectEq("float4(-0f) == float4(0f)", true, float4(-0f) == float4(0f));
Assert.expectEq("float4(1f) == float4(2f)", false, float4(1f) == float4(2f));
Assert.expectEq("float4(2f) == float4(1f)", false, float4(2f) == float4(1f));

Assert.expectEq("function float4(1f) == float4(1f)", true, check(float4(1f), float4(1f)));
Assert.expectEq("function float4(0f) == float4(0f)", true, check(float4(0f), float4(0f)));
Assert.expectEq("function float4(0f) == float4(-0f)", true, check(float4(0f), float4(-0f)));
Assert.expectEq("function float4(0f) == float4(-0f)", true, check(float4(-0f), float4(0f)));
Assert.expectEq("function float4(1f) == float4(2f)", false, check(float4(1f), float4(2f)));
Assert.expectEq("function float4(2f) == float4(1f)", false, check(float4(2f), float4(1f)));


Assert.expectEq("float4(1f, 2f, 3f, 4f) == float4(0f, 2f, 3f, 4f)", false, float4(1f, 2f, 3f, 4f) == float4(0f, 2f, 3f, 4f));
Assert.expectEq("float4(1f, 1f, 3f, 4f) == float4(0f, 2f, 3f, 4f)", false, float4(1f, 2f, 3f, 4f) == float4(1f, 1f, 3f, 4f));
Assert.expectEq("float4(1f, 2f, 2f, 4f) == float4(0f, 2f, 3f, 4f)", false, float4(1f, 2f, 3f, 4f) == float4(1f, 2f, 2f, 4f));
Assert.expectEq("float4(1f, 2f, 3f, 5f) == float4(0f, 2f, 3f, 4f)", false, float4(1f, 2f, 3f, 4f) == float4(1f, 2f, 3f, 5f));

Assert.expectEq("function float4(1f, 2f, 3f, 4f) == float4(0f, 2f, 3f, 4f)", false, check(float4(1f, 2f, 3f, 4f), float4(0f, 2f, 3f, 4f)));
Assert.expectEq("function float4(1f, 1f, 3f, 4f) == float4(0f, 2f, 3f, 4f)", false, check(float4(1f, 2f, 3f, 4f), float4(1f, 1f, 3f, 4f)));
Assert.expectEq("function float4(1f, 2f, 2f, 4f) == float4(0f, 2f, 3f, 4f)", false, check(float4(1f, 2f, 3f, 4f), float4(1f, 2f, 2f, 4f)));
Assert.expectEq("function float4(1f, 2f, 3f, 5f) == float4(0f, 2f, 3f, 4f)", false, check(float4(1f, 2f, 3f, 4f), float4(1f, 2f, 3f, 5f)));

// Bug 698365 - float4 mixed-representation equality incorrect
// https://bugzilla.mozilla.org/show_bug.cgi?id=698365
AddStrictTestCase("float4(1,1,1,1) == 1", true, float4(1,1,1,1) == 1);
AddStrictTestCase("float4(1,1,1,1) == 1 check()", true, check(float4(1,1,1,1), 1));
var one = 1;
AddStrictTestCase("float4(1,1,1,1) == one", true, float4(1,1,1,1) == one);
AddStrictTestCase("float4(1,1,1,1) == one check()", true, check(float4(1,1,1,1), one));

AddStrictTestCase("float4(2.125, 2.125, 2.125, 2.125) == 2.125", true, float4(2.125, 2.125, 2.125, 2.125) == 2.125);
AddStrictTestCase("float4(2.125, 2.125, 2.125, 2.125) == 2.125 check()", true, check(float4(2.125, 2.125, 2.125, 2.125), 2.125));
AddStrictTestCase("2.125 == float4(2.125, 2.125, 2.125, 2.125)", true, 2.125 == float4(2.125, 2.125, 2.125, 2.125));
AddStrictTestCase("2.125 == float4(2.125, 2.125, 2.125, 2.125) check()", true, check(2.125, float4(2.125, 2.125, 2.125, 2.125)));

AddStrictTestCase("float4(1,1,1,1) == true", true, float4(1,1,1,1) == true);
AddStrictTestCase("float4(1,1,1,1) == true check()", true, check(float4(1,1,1,1), true));
var _true = true;
AddStrictTestCase("float4(1,1,1,1) == _true", true, float4(1,1,1,1) == _true);
AddStrictTestCase("float4(1,1,1,1) == _true check()", true, check(float4(1,1,1,1), _true));



