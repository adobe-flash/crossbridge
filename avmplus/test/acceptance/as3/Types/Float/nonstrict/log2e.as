/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.11";
// var VERSION = "AS3";
// var TITLE   = "float.LOG2E";



var flt_log2e:float = float(1.44269504);

Assert.expectEq("float.LOG2E", flt_log2e, float.LOG2E);
Assert.expectEq("typeof float.LOG2E", "float", getQualifiedClassName(float.LOG2E));

Assert.expectEq("float.LOG2E - DontDelete", false, delete(float.LOG2E));
Assert.expectEq("float.LOG2E is still ok", flt_log2e, float.LOG2E);

Assert.expectEq("float.LOG2E - DontEnum", '',getFloatProp('LOG2E'));
Assert.expectEq("float.LOG2E is no enumberable", false, float.propertyIsEnumerable('LOG2E'));

Assert.expectError("float.LOG2E - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.LOG2E = 0; });
Assert.expectEq("float.LOG2E is still here", flt_log2e, float.LOG2E);


