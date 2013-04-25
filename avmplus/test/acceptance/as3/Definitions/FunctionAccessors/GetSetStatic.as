/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import GetSetStaticPackage.*;
import GetSetStaticSameName.*;
import com.adobe.test.Assert;

// var SECTION = "FunctionAccessors";
// var VERSION = "AS3";
// var TITLE   = "Function Accessors";
var BUGNUMBER = "";


Assert.expectEq("Static getter var:int", -10, GetSetStatic.y);
Assert.expectEq("Static setter var:int", 23334, (GetSetStatic.y = 23334, GetSetStatic.y));
Assert.expectEq("Static getter var:Array", "1,2,3", GetSetStatic.x.toString());
Assert.expectEq("Static setter var:Array", "4,5,6", (GetSetStatic.x = new Array(4,5,6), GetSetStatic.x.toString()));
Assert.expectEq("Static getter var:Boolean", true, GetSetStatic.boolean);
Assert.expectEq("Static setter var:Boolean", false, (GetSetStatic.boolean = false, GetSetStatic.boolean));
Assert.expectEq("Static getter var:uint", 1, GetSetStatic.u);
Assert.expectEq("Static setter var:uint", 42, (GetSetStatic.u = 42, GetSetStatic.u));
Assert.expectEq("Static getter var:String", "myString", GetSetStatic.string);
Assert.expectEq("Static setter var:String", "new string", (GetSetStatic.string = "new string", GetSetStatic.string));

// Attempt to access the private vars directly
// New instances of vars will be created since the private vars are not accessible
try{
    var res = "not run";
    Assert.expectEq("Access private var:Array", 4, GetSetStatic._x = 4);
    res = "no exception";
} catch (e1) {
    res = "exception";
} finally {
    Assert.expectEq("Access private var:Array", "no exception", res);
}

try{
    var res = "not run";
    Assert.expectEq("Access private var:int", 4, GetSetStatic._y = 4);
    res = "no exception";
} catch (e2) {
    res = "exception";
} finally {
    Assert.expectEq("Access private var:int", "no exception", res);
}

try{
    var res = "not run";
    Assert.expectEq("Access private var:Boolean", 4, GetSetStatic._b = 4);
    res = "no exception";
} catch (e3) {
    res = "exception";
} finally {
    Assert.expectEq("Access private var:Boolean", "no exception", res);
}

try{
    var res = "not run";
    Assert.expectEq("Access private var:uint", 4, GetSetStatic._u = 4);
    res = "no exception";
} catch (e4) {
    res = "exception";
} finally {
    Assert.expectEq("Access private var:uint", "no exception", res);
}

try{
    var res = "not run";
    Assert.expectEq("Access private var:String", 4, GetSetStatic._s = 4);
    res = "no exception";
} catch (e5) {
    res = "exception";
} finally {
    Assert.expectEq("Access private var:String", "no exception", res);
}

// call setter from setter
GetSetStatic.sfs2 = 55;
Assert.expectEq("Call setter from setter", 55, GetSetStatic.sfs1);
Assert.expectEq("Call setter from setter", 55, GetSetStatic.sfs2);

// call setter from getter
Assert.expectEq("Call setter from getter", 0, GetSetStatic.sfg2);
Assert.expectEq("Call setter from getter", "PASSED", GetSetStatic.sfg1);

// call getter from setter
GetSetStatic.gfs1 = "FAILED";// setter for gfs1 should make the string 'PASSED'
Assert.expectEq("Call getter from setter", "PASSED", GetSetStatic.gfs1);
Assert.expectEq("Call getter from setter", "PASSED", GetSetStatic.gfs2);

// call getter from getter

Assert.expectEq("Call getter from getter", "PASSED", GetSetStatic.gfg1);
Assert.expectEq("Call getter from getter", "PASSED", GetSetStatic.gfg2);

// Try to access a getter in a class that's the same name as the package it's in
// See bug 133422; test case needs to be updated when/if bug is fixed
try{
    var res = "not run";
    trace(GetSetStaticSameName.y);
    res = "no exception";
} catch (e6) {
    res = "exception";
} finally {
    Assert.expectEq("Access getter in class in package of same name", "exception", res);
}

// Attempt to access non-static var
// See bug 117661; test case needs to be updated when/if bug is fixed
try{
    var res = "not run";
    var someVar = GetSetStatic.n;
    res = "no exception";
} catch (e7) {
    res = "exception";
} finally {
    Assert.expectEq("Get non-static var:Number", "exception", res);
}

// Set non-static var
// See bug 133468; test case needs to be updated when/if bug is fixed
try{
    var res = "not run";
    GetSetStatic.n = 5.55;
    res = GetSetStatic.n;
} catch (e8) {
    res = "exception";
} finally {
    Assert.expectEq("Set non-static var:Number", 5.55, res);
}


