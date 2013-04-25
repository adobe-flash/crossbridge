/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// Bugzilla 708300

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Strict: Type inference feeds into comparison";


// -- begin test

function f() 
{
    // The bug here was that the type of (v + 1), though demonstrably
    // float, was not being propagated properly during analysis.

	var v:float = 0;
	var g:* =   (v + 1) > v;  // ASC errors out on this one

    // Check the others too
	var g2:* =   (v - 1) > v;  // ASC errors out on this one
	var g3:* =   (v * 1) > v;  // ASC errors out on this one
	var g4:* =   (v / 1) > v;  // ASC errors out on this one
	var g5:* =   (v % 1) > v;  // ASC errors out on this one

    // Similar unary case (was not failing, just safeguarding)

	var h:* = (- - - v) > v;
	var k:* = (+ + + v) > v;
}

// -- end test

Assert.expectEq("Trivially true", true, true);

