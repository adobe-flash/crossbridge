/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.6.1";
// var VERSION = "AS3";
// var TITLE   = "The != operator augmented by float4 values";


function check(param1:*, param2:*):* { return param1 != param2; }

Assert.expectEq("float4(1f) != float4(1f)", false, float4(1f) != float4(1f));
Assert.expectEq("float4(0f) != float4(0f)", false, float4(0f) != float4(0f));
Assert.expectEq("float4(0f) != float4(-0f)", false, float4(0f) != float4(-0f));
Assert.expectEq("float4(-0f) != float4(0f)", false, float4(-0f) != float4(0f));
Assert.expectEq("float4(1f) != float4(2f)", true, float4(1f) != float4(2f));
Assert.expectEq("float4(2f) != float4(1f)", true, float4(2f) != float4(1f));

Assert.expectEq("function float4(1f) != float4(1f)", false, check(float4(1f), float4(1f)));
Assert.expectEq("function float4(0f) != float4(0f)", false, check(float4(0f), float4(0f)));
Assert.expectEq("function float4(0f) != float4(-0f)", false, check(float4(0f), float4(-0f)));
Assert.expectEq("function float4(-0f) != float4(0f)", false, check(float4(-0f), float4(0f)));
Assert.expectEq("function float4(1f) != float4(2f)", true, check(float4(1f), float4(2f)));
Assert.expectEq("function float4(2f) != float4(1f)", true, check(float4(2f), float4(1f)));



Assert.expectEq("float4(1f, 2f, 3f, 4f) != float4(0f, 2f, 3f, 4f)", true, float4(1f, 2f, 3f, 4f) != float4(0f, 2f, 3f, 4f));
Assert.expectEq("float4(1f, 1f, 3f, 4f) != float4(0f, 2f, 3f, 4f)", true, float4(1f, 2f, 3f, 4f) != float4(1f, 1f, 3f, 4f));
Assert.expectEq("float4(1f, 2f, 2f, 4f) != float4(0f, 2f, 3f, 4f)", true, float4(1f, 2f, 3f, 4f) != float4(1f, 2f, 2f, 4f));
Assert.expectEq("float4(1f, 2f, 3f, 5f) != float4(0f, 2f, 3f, 4f)", true, float4(1f, 2f, 3f, 4f) != float4(1f, 2f, 3f, 5f));

Assert.expectEq("function float4(1f, 2f, 3f, 4f) != float4(0f, 2f, 3f, 4f)", true, check(float4(1f, 2f, 3f, 4f), float4(0f, 2f, 3f, 4f)));
Assert.expectEq("function float4(1f, 1f, 3f, 4f) != float4(0f, 2f, 3f, 4f)", true, check(float4(1f, 2f, 3f, 4f), float4(1f, 1f, 3f, 4f)));
Assert.expectEq("function float4(1f, 2f, 2f, 4f) != float4(0f, 2f, 3f, 4f)", true, check(float4(1f, 2f, 3f, 4f), float4(1f, 2f, 2f, 4f)));
Assert.expectEq("function float4(1f, 2f, 3f, 5f) != float4(0f, 2f, 3f, 4f)", true, check(float4(1f, 2f, 3f, 4f), float4(1f, 2f, 3f, 5f)));


