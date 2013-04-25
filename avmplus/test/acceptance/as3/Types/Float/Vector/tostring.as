/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector.toString with float";



var v2:Vector.<float>=new Vector.<float>;
v2.push(float(1.1));v2.push(new float(3.14));v2.push(99.99);
Assert.expectEq(
         "small vector of floats toString",
         "1.100000023841858,3.140000104904175,99.98999786376953",
         v2.toString() );
