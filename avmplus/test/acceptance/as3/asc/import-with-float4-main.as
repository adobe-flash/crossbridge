/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Bugzilla 707902

// This is the main code, it needs to be compiled with an -import of
// import-with-float4-helper.abc.  When running, we should be running
// with -ea, the bug was partly that there was an assertion to flag
// the problem, but with assertions disabled we won't see that.

// var SECTION = "ASC";
// var VERSION = "AS3";
// var TITLE   = "Allow an ABC import to contain a float4 default argument";


// -- begin test

import fnord.*;
var xyzzy = function() { return f() };  // Do nothing interesting, it is the parsing of the imported file that is the problem.
print(xyzzy());
import com.adobe.test.Assert;

// -- end test

Assert.expectEq("Trivially true", true, true);

