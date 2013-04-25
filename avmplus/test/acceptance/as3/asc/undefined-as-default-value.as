/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 707661

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Strict: Allow undefined as a default value";


// -- begin test

// Traditionally these have been allowed, even if they don't make a lot of sense.

var x1:Number = undefined;
var x2:int = undefined;
var x3:Object = undefined;
var x4:Date = undefined;
var x5:String = undefined;
var x6:uint = undefined;
var x7:float = undefined;
var x8:float4 = undefined;
var x9:Boolean = undefined;

// -- end test

Assert.expectEq("Trivially true", true, true);

