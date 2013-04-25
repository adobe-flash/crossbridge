/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "AS3";  // Version of JavaScript or ECMA
// var TITLE   = "Interface Definition";       // Provide ECMA section title or a description
var BUGNUMBER = "";


//-----------------------------------------------------------------------------

import TraitsNotAccessible.*;

import com.adobe.test.Assert;
import com.adobe.test.Utils;
var eg = new TraitTest();
Assert.expectEq("call implemented method", "x.I::f()", eg.doCall());
var thisError : String = "no exception thrown";
var result = "";
try {
    result = eg.doSuperCall();
} catch (e) {
    thisError = e.toString();
} finally {
    Assert.expectEq("call super method", Utils.REFERENCEERROR+1070, Utils.referenceError(thisError));

}

              // displays results.
