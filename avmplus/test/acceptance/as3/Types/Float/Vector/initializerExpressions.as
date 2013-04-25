/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "../floatUtil.as";


// var SECTION = "";
// var VERSION = "AS3";
// var TITLE   = "Vector initializer expressions with float";



Assert.expectEq(
    "length property returns expected value for vector<float>",
    9,
    new <float> [0,1,2,3,4,5,6,7,8].length);


Assert.expectEq(
    "pop float element from Vector initializer",
    float(3.15),
    new<float>[5.15, 4.15,3.15].pop());

Assert.expectEq(
    "push float element into Vector initializer",
    6,
    new<float>[0.3,.56,.12,3.14].push(float(4500),float(.0001))); // TODO: use float literals



