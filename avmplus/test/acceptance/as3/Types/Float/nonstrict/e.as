/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.8";
// var VERSION = "AS3";
// var TITLE   = "float.E";



// var flt_e:float = float(Math.E);
var flt_e:float = float(2.7182818);

Assert.expectEq("float.E", flt_e, float.E);
Assert.expectEq("typeof float.E", "float", getQualifiedClassName(float.E));

Assert.expectEq("float.E - DontDelete", false, delete(float.E));
Assert.expectEq("float.E is still ok", flt_e, float.E);

Assert.expectEq("float.E - DontEnum", '',getFloatProp('E'));
Assert.expectEq("float.E is no enumberable", false, float.propertyIsEnumerable('E'));

Assert.expectError("float.E - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.E = 0; });
Assert.expectEq("float.E is still here", flt_e, float.E);


