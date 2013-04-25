/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "Misc Tests";
// var VERSION = "";
// var TITLE = "Verify that timeout throws Error #1502";


var expected = 'Error: Error #1502';

try {
    while(true)
    {
    }
} catch (err) {
    result1 = Utils.parseError(err.toString(), expected.length);
} finally {
    Assert.expectEq('Script timeout test', expected, result1);
}


