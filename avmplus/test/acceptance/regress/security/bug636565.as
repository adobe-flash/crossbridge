/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
import com.adobe.test.Utils;
import p.q.*;

var errorMsg:String="";
try {
    f=function() {
        return x;
    }
    f();
} catch(e) {
    errorMsg=Utils.grabError(e,e.toString());
}
Assert.expectEq("Bug 636565: Test for non-crash in ambiguous resolution", "Error #1065", errorMsg); 

