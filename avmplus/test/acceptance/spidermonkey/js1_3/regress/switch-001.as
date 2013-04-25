/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'switch-001.js';

/**
   File Name:          switch-001.js
   Section:
   Description:

   http://scopus.mcom.com/bugsplat/show_bug.cgi?id=315767

   Verify that switches do not use strict equality in
   versions of JavaScript < 1.4.  It's now been decided that
   we won't put in version switches, so all switches will
   be ECMA.

   Author:             christine@netscape.com
   Date:               12 november 1997
*/
// var SECTION = "switch-001";
// var VERSION = "JS1_3";
// var TITLE   = "switch-001";
var BUGNUMBER="315767";


result = "fail:  did not enter switch";

switch (true) {
case 1:
  result = "fail:  version 130 should force strict equality";
  break;
case true:
  result = "pass";
  break;
default:
  result = "fail: evaluated default statement";
}

Assert.expectEq(
    
  "switch / case should use strict equality in version of JS < 1.4",
  "pass",
  result );


