/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.5.5";
// var VERSION = "AS3";
// var TITLE   = "The <, >, <= and >= operators";


var onef:float4 = new float4(1f);
var twof:float4 = new float4(2f);

function lessThan(val1:*, val2:*):* { return (val1 < val2); }
function greaterThan(val1:*, val2:*):* { return (val1 > val2); }
function lessThanOrEqual(val1:*, val2:*):* { return (val1 <= val2); }
function greaterThanOrEqual(val1:*, val2:*):* { return (val1 >= val2); }

Assert.expectEq("float4(1f) < float4(2f)", false, float4(1f) < float4(2f));
Assert.expectEq("float4(2f) < float4(1f)", false, float4(2f) < float4(1f));
Assert.expectEq("float4(2f) < float4(2f)", false, float4(2f) < float4(2f));
Assert.expectEq("lessThan(float4(1f), float4(2f))", false, lessThan(float4(1f), float4(2f)));
Assert.expectEq("lessThan(float4(2f), float4(1f))", false, lessThan(float4(2f), float4(1f)));
Assert.expectEq("lessThan(float4(2f), float4(2f))", false, lessThan(float4(2f), float4(2f)));

Assert.expectEq("float4(1f) > float4(2f)", false, float4(1f) > float4(2f));
Assert.expectEq("float4(2f) > float4(1f)", false, float4(2f) > float4(1f));
Assert.expectEq("float4(2f) > float4(2f)", false, float4(2f) > float4(2f));
Assert.expectEq("greaterThan(float4(1f), float4(2f))", false, greaterThan(float4(1f), float4(2f)));
Assert.expectEq("greaterThan(float4(2f), float4(1f))", false, greaterThan(float4(2f), float4(1f)));
Assert.expectEq("greaterThan(float4(2f), float4(2f))", false, greaterThan(float4(2f), float4(2f)));

Assert.expectEq("float4(1f) <= float4(2f)", false, float4(1f) <= float4(2f));
Assert.expectEq("float4(2f) <= float4(1f)", false, float4(2f) <= float4(1f));
Assert.expectEq("float4(2f) <= float4(2f)", false, float4(2f) <= float4(2f));
Assert.expectEq("lessThanOrEqual(float4(1f), float4(2f))", false, lessThanOrEqual(float4(1f), float4(2f)));
Assert.expectEq("lessThanOrEqual(float4(2f), float4(1f))", false, lessThanOrEqual(float4(2f), float4(1f)));
Assert.expectEq("lessThanOrEqual(float4(2f), float4(2f))", false, lessThanOrEqual(float4(2f), float4(2f)));

Assert.expectEq("float4(1f) >= float4(2f)", false, float4(1f) >= float4(2f));
Assert.expectEq("float4(2f) >= float4(1f)", false, float4(2f) >= float4(1f));
Assert.expectEq("float4(2f) >= float4(2f)", false, float4(2f) >= float4(2f));
Assert.expectEq("greaterThanOrEqual(float4(1f), float4(2f))", false, greaterThanOrEqual(float4(1f), float4(2f)));
Assert.expectEq("greaterThanOrEqual(float4(2f), float4(1f))", false, greaterThanOrEqual(float4(2f), float4(1f)));
Assert.expectEq("greaterThanOrEqual(float4(2f), float4(2f))", false, greaterThanOrEqual(float4(2f), float4(2f)));


