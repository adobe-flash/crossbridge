/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.1.1";
// var VERSION = "AS3";
// var TITLE   = "NumericLiteral";


var flt:float = 2.7182818284e5f;
Assert.expectEq("test float against FloatLiteral", 27182818.284e-2f, flt);

var val:* = 2.7182818284e5f;
Assert.expectEq("test FloatLiteral against float", flt, val);


