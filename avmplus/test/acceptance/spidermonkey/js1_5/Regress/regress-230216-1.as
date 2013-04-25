/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-230216-1.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 230216;
var summary = 'check for numerical overflow in regexps in back reference and bounds for {} quantifier';
var actual = '';
var expect = '';
var status = '';

 function inSection(x) {
   return "Section "+x+" of test -";
}

status = inSection(1) + ' check for overflow in backref';

actual = 'undefined';
expect = false;

try {
    actual = /(a)\21474836481/.test("aa");
} catch(e) {
    status += ' Error: ' + e;
}

Assert.expectEq(status, expect, actual);

status = inSection(1.1) + ' check for overflow in backref';

actual = 'undefined';
expect = false;

try {
    var i = 21474836481;
    actual = new RegExp('(a)\\' + i).test("aa");
} catch(e) {
    status += ' Error: ' + e;
}

Assert.expectEq(status, expect, actual);

status = inSection(1.2) + ' check for overflow in backref';

actual = 'undefined';
expect = false;

try {
    actual = /a{21474836481}/.test("a")
} catch(e) {
    status += ' Error: ' + e;
}

Assert.expectEq(status, expect, actual);


status = inSection(2) + ' check for overflow in backref';

actual = 'undefined';
expect = false;

try {
    actual = /a\21474836480/.test("");
} catch(e) {
    status += ' Error: ' + e;
}

Assert.expectEq(status, expect, actual);

status = inSection(3) + ' check for overflow in backref';

actual = 'undefined';
expect = ["ax", "ax", "", "a"].toString();

try {
    pattern = /((\3|b)\2(a)x)+/;
    string = 'aaxabxbaxbbx';
    actual = pattern(string).toString();
} catch(e) {
    status += ' Error: ' + e;
}

Assert.expectEq(status, expect, actual);

