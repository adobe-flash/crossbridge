/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.Domain
import avmplus.System
import com.adobe.test.Assert;

var domain = Domain.currentDomain;

// var SECTION = "regression test";
// var VERSION = "as3";
// var TITLE   = "Regression test for bug 618215";

var support_abc:String = System.argv[0];

var err = "";
try {
    domain.load(support_abc);
} catch (e) {
    err = e.toString();
}

Assert.expectEq("Fuzzed file should throw kIllegalOperandTypeError", "TypeError: Error #1034", err.substring(0,22));

