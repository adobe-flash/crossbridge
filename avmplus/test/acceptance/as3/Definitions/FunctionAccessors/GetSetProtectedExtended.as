/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import GetSetProtectedExtended.*;
import com.adobe.test.Assert;

// var SECTION = "FunctionAccessors";
// var VERSION = "AS3";
// var TITLE   = "Function Accessors";
var BUGNUMBER = "";


OBJ = new GetSetProtectedExtended();

/*
 *
 * These will all need to be changed when
 * the protected attribute is implemented.
 *
 *
 */


Assert.expectEq("Protected getter, boolean", true, OBJ.getBoolean());
Assert.expectEq("Protected setter, boolean", false, OBJ.setBoolean(false));
Assert.expectEq("Protected getter, uint", 101, OBJ.getUint());
Assert.expectEq("Protected setter, uint", 5, OBJ.setUint(5));
Assert.expectEq("Protected getter, array", "1,2,3", OBJ.getArray().toString());
Assert.expectEq("Protected setter, array", "one,two,three", OBJ.setArray(["one","two","three"]).toString());
Assert.expectEq("Protected getter, string", "myString", OBJ.getString());
Assert.expectEq("Protected setter, string", "new string", OBJ.setString("new string"));
Assert.expectEq("Protected getter, no type", "no type", OBJ.getNoType());
Assert.expectEq("Protected setter, no type", 2012, OBJ.setNoType(2012));


