/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 706399

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Strict: Allow arithmetic on float4 and Number";


// -- begin test

var x:float4 = float4(0);
var y:Number = 37;
var z1 = x+y;                    // This should always be allowed
var z1 = x-y;                    // This should always be allowed
var z2 = x*y;                    // This should always be allowed
var z3 = x/y;                    // This should always be allowed
var z4 = x%y;                    // This should always be allowed

var w1 = y+x;                    // This should always be allowed
var w1 = y-x;                    // This should always be allowed
var w2 = y*x;                    // This should always be allowed
var w3 = y/x;                    // This should always be allowed
var w4 = y%x;                    // This should always be allowed

// -- end test

Assert.expectEq("Trivially true", true, true);

