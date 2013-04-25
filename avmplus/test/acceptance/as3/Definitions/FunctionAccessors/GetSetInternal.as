/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import GetSetInternal.*;
import com.adobe.test.Assert;

// var SECTION = "FunctionAccessors";
// var VERSION = "AS3";
// var TITLE   = "Function Accessors";
var BUGNUMBER = "";


OBJ = new GetSetInternal();

Assert.expectEq("Internal getter, boolean", true, OBJ.getBoolean());
Assert.expectEq("Internal setter, boolean", false, OBJ.setBoolean(false));
Assert.expectEq("Internal getter, uint", 101, OBJ.getUint());
Assert.expectEq("Internal setter, uint", 5, OBJ.setUint(5));
Assert.expectEq("Internal getter, array", "1,2,3", OBJ.getArray().toString());
Assert.expectEq("Internal setter, array", "one,two,three", OBJ.setArray(["one","two","three"]).toString());
Assert.expectEq("Internal getter, string", "myString", OBJ.getString());
Assert.expectEq("Internal setter, string", "new string", OBJ.setString("new string"));
Assert.expectEq("Internal getter, no type", "no type", OBJ.getNoType());
Assert.expectEq("Internal setter, no type", 2012, OBJ.setNoType(2012));

try {
    internObj = new GetSetInternalInternal();
    res = "no exception";
} catch (e1) {
    res = "exception";
}
Assert.expectEq("Try to instantiate internal class from outside package", "exception", res);




