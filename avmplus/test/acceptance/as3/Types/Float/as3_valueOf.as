/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.7.2";
// var VERSION = "AS3";
// var TITLE   = "AS3 function valueOf ( ) : float";


var test_flt:float = 2.7182818284e5f;

Assert.expectEq("float.AS3::valueOf = function", true, test_flt.AS3::valueOf is Function);
Assert.expectEq("float.AS3::valueOf returns a float", "float", getQualifiedClassName(test_flt.AS3::valueOf()));

Assert.expectEq("float.AS3::valueOf", test_flt, test_flt.AS3::valueOf());
Assert.expectEq("float.AS3::valueOf FloatLiteral", test_flt.AS3::valueOf(), 2.7182818284e5f.AS3::valueOf());



