/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-333728.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 333728;
var summary = 'Throw ReferenceErrors for typeof(...undef)';
var actual = '';
var expect = 'undefined';

//printBugNumber(BUGNUMBER);
//printStatus(summary);

try {
  actual = typeof(0, undef);
} catch(ex) {
  actual = ex.name;
}

Assert.expectEq(summary + ': typeof (0, undef)', expect, actual.toString());

try {
  actual = typeof(0 || undef);
} catch(ex) {
  actual = ex.name;
}

Assert.expectEq(summary + ': typeof (0 || undef)', expect, actual.toString());

try {
  actual = typeof(1 && undef);
} catch(ex) {
  actual = ex.name;
}

Assert.expectEq(summary + ': typeof (1 && undef)', expect, actual.toString());

try {
  actual = typeof(0 ? 0 : undef);
} catch(ex) {
  actual = ex.name;
}

Assert.expectEq(summary + ': typeof (0 ? 0 : undef)', expect, actual.toString());

try {
  actual = typeof(1 ? undef: 0);
} catch(ex) {
  actual = ex.name;
}

Assert.expectEq(summary + ': typeof (1 ? undef : 0)', expect, actual.toString());

try {
  actual = typeof(!this ? 0 : undef);
} catch(ex) {
  actual = ex.name;
}

Assert.expectEq(summary + ': typeof (!this ? 0 : undef)', expect, actual.toString());

