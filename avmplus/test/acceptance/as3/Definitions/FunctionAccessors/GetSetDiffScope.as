/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

 import GetSetDiffScope.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;


//  var SECTION = "FunctionAccessors";
//  var VERSION = "AS3";
//  var TITLE   = "Function Accessors";
 var BUGNUMBER = "143360";




w = new GetSetDiffScope();


try {
    w.g1 = {a:1, b:2, c:3};
    errorNumber = -1;
    errorType = undefined;
} catch(e1) {
    errorNumber = Utils.referenceError(e1.toString());
    errorType = e1 is ReferenceError;

}


Assert.expectEq("Setter is missing","ReferenceError: Error #1074", errorNumber);
Assert.expectEq("Make sure property value hasn't changed", "original value", w.g1);


try {
    w.g2 = {a:1, b:2, c:3};
    errorNumber = -1;
    errorType = undefined;
} catch(e2) {
    errorNumber = Utils.referenceError(e2.toString());
    errorType = e2 is ReferenceError;

}

Assert.expectEq("Setter is private", "ReferenceError: Error #1074", errorNumber);
Assert.expectEq("Make sure property value hasn't changed", 5, w.g2);


try {
    somevar = w.s1;
    errorNumber = -1;
    errorType = undefined;
} catch(e3) {
    errorNumber = Utils.referenceError(e3.toString());
    errorType = e3 is ReferenceError;

}

Assert.expectEq("Setter is missing, error 1069", "ReferenceError: Error #1069", errorNumber);
Assert.expectEq("Checking for correct error type ReferenceError", true, errorType);

try {
    somevar = w.s2;
    errorNumber = -1;
    errorType = undefined;
} catch(e4) {
    errorNumber = Utils.referenceError(e4.toString());
    errorType = e4 is ReferenceError;

}

Assert.expectEq("Setter is private, error 1077", "ReferenceError: Error #1077", errorNumber);
Assert.expectEq("Checking for correct error type ReferenceError", true, errorType);

var t:testclass = new testclass();

try {
    somevar = t.doGet();
    errorNumber = -1;
    errorType = undefined;
} catch (e1) {
    errorNumber = Utils.referenceError(e1.toString());
    errorType = e1 is ReferenceError;
}
Assert.expectEq("Public getter, namespace setter; get", 1, somevar);
Assert.expectEq("Checking error did not occur, error number", -1, errorNumber);
Assert.expectEq("Checking error did not occur, error type", undefined, errorType);

try {
    somevar = t.doSet();
    errorNumber = -1;
    errorType = undefined;
} catch (e2) {
    errorNumber = Utils.referenceError(e2.toString());
    errorType = e2 is ReferenceError;
}

Assert.expectEq("Public getter, namespace setter, set; error 1074", "ReferenceError: Error #1074", errorNumber);
Assert.expectEq("Checking for correct error type ReferenceError", true, errorType);


