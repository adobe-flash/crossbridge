/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-309242.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 309242;
var summary = 'E4X should be on by default while preserving comment hack';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

/*
    E4X should be available regardless of script type
    <!-- and --> should begin comment to end of line
    unless type=text/javascript;e4x=1
*/

expect = true;
actual = false;
// the next line will be ignored when e4x is not requested
<!-- comment -->; actual = true;

Assert.expectEq(summary + ': &lt;!-- is comment to end of line', expect, actual);

expect = true;
actual = true;
// the next line will be ignored when e4x is not requested
<!--
 actual = false;
// -->

Assert.expectEq(summary + ': comment hack works inside script', expect, actual);

// E4X is available always

var x = <foo/>;

expect = 'element';
actual = x.nodeKind();

Assert.expectEq(summary + ': E4X is available', expect, actual);

