/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.3.3";
// var VERSION = "AS3";
// var TITLE   = "The void operator";


var flt:float = 2.7182818284e5f;
Assert.expectEq("void on float", undefined, void(flt));
Assert.expectEq("float value is not altered", 2.7182818284e5f, flt);
Assert.expectEq("void on FloatLiteral", undefined, void(2.7182818284e5f));
Assert.expectEq("void on float.MAX_VALUE", undefined, void(float.MAX_VALUE));


