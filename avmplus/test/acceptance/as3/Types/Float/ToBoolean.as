/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "9.7";
// var VERSION = "AS3";
// var TITLE   = "The operation ToBoolean";


var v:Boolean;

v = float(+0.0);
Assert.expectEq("ToBoolean(+0.0f)", false, v);

v = +0.0f;
Assert.expectEq("ToBoolean(+0.0f) FloatLiteral", false, v);

v = float(-0.0);
Assert.expectEq("ToBoolean(-0.0f)", false, v);

v = -0.0f;
Assert.expectEq("ToBoolean(-0.0f) FloatLiteral", false, v);

v = float.NaN;
Assert.expectEq("ToBoolean(float.NaN)", false, v);

v = float.MIN_VALUE;
Assert.expectEq("ToBoolean(float.MIN_VALUE)", true, v);

v = -float.MIN_VALUE;
Assert.expectEq("ToBoolean(-float.MIN_VALUE)", true, v);

v = float(-0.23);
Assert.expectEq("ToBoolean(-0.23f)", true, v);

v = -0.23f;
Assert.expectEq("ToBoolean(-0.23f) FloatLiteral", true, v);



