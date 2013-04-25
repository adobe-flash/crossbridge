/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.4.2";
// var VERSION = "AS3";
// var TITLE   = "The / operation agumented by float values";



var f1:float = 1.2345678e9; // TODO: use float literals
var f2:float = 6.543210987; // TODO: use float literals
// c double 188679197.7903248965740204
// c float  188679200
Assert.expectEq("division on float & number should yield different results", true, (f1/f2) != (1.2345678e9/6.543210987) );
Assert.expectEq("division on float & number should yield different results", true, (f1/f2) != (Number(f1)/Number(f2)) );


