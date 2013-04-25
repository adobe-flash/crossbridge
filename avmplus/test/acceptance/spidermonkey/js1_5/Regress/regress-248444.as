/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-248444.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 248444;
var summary = 'toString/toSource of RegExp should escape slashes';
var actual = '';
var expect = '';

function inSection(x) {
   return "Section "+x+" of test -";
}

var re;
expect = '/[^\\/]+$/';

status = summary + ' ' + inSection(1);
re = /[^\/]+$/;
actual = re.toString();
Assert.expectEq(status, expect, actual);

status = summary + ' ' + inSection(2);
re = RegExp("[^\\\/]+$");
actual = re.toString();
Assert.expectEq(status, expect, actual);

status = summary + ' ' + inSection(3);
re = RegExp("[^\\/]+$");
actual = re.toString();
Assert.expectEq(status, expect, actual);

status = summary + ' ' + inSection(4);
re = RegExp("[^\/]+$");
actual = re.toString();
Assert.expectEq(status, expect, actual);

status = summary + ' ' + inSection(5);
re = RegExp("[^/]+$");
actual = re.toString();
Assert.expectEq(status, expect, actual);



