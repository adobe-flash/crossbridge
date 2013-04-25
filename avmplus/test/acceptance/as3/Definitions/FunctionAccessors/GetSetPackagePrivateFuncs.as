/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import GetSetPackagePrivateFuncs.*;
import com.adobe.test.Assert;

// var SECTION = "FunctionAccessors";
// var VERSION = "AS3";
// var TITLE   = "Function Accessors";
var BUGNUMBER = "";


var OBJ = new GetSetPackagePrivateFuncs();


// Attempt to access package private getters and setters
try{
    var res = "not run";
    OBJ.y;
    res = "no exception";
} catch (e1) {
    res = "exception";
} finally {
    Assert.expectEq("Get package private var:int", "exception", res);
}

try{
    var res = "not run";
    OBJ.y = 23334;
    OBJ.y;
    res = "no exception";
} catch (e2) {
    res = "exception";
} finally {
    Assert.expectEq("Set package private var:int", "exception", res);
}

try{
    var res = "not run";
    OBJ.x.toString();
    res = "no exception";
} catch (e3) {
    res = "exception";
} finally {
    Assert.expectEq("Get package private var:Array", "exception", res);
}

try{
    var res = "not run";
    OBJ.x = new Array(4,5,6);
    OBJ.x.toString();
    res = "no exception";
} catch (e3) {
    res = "exception";
} finally {
    Assert.expectEq("Set package private var:Array", "exception", res);
}

try{
    var res = "not run";
    OBJ.boolean;
    res = "no exception";
} catch (e4) {
    res = "exception";
} finally {
    Assert.expectEq("Get package private var:Boolean", "exception", res);
}

try{
    var res = "not run";
    OBJ.boolean = false;
    OBJ.boolean;
    res = "no exception";
} catch (e5) {
    res = "exception";
} finally {
    Assert.expectEq("Set package private var:Boolean", "exception", res);
}


try{
    var res = "not run";
    OBJ.u;
    res = "no exception";
} catch (e2) {
    res = "exception";
} finally {
    Assert.expectEq("Get package private var:uint", "exception", res);
}

try{
    var res = "not run";
    OBJ.u = 42;
    OBJ.u;
    res = "no exception";
} catch (e3) {
    res = "exception";
} finally {
    Assert.expectEq("Set package private var:uint", "exception", res);
}

try{
    var res = "not run";
    OBJ.string;
    res = "no exception";
} catch (e3) {
    res = "exception";
} finally {
    Assert.expectEq("Get package private var:String", "exception", res);
}

try{
    var res = "not run";
    OBJ.string = "new string";
    OBJ.string;
    res = "no exception";
} catch (e4) {
    res = "exception";
} finally {
    Assert.expectEq("Set package private var:String", "exception", res);
}

try{
    var res = "not run";
    OBJ.noType;
    res = "no exception";
} catch (e3) {
    res = "exception";
} finally {
    Assert.expectEq("Get package private untyped var", "exception", res);
}

try{
    var res = "not run";
    OBJ.noType = "new value";
    OBJ.noType;
    res = "no exception";
} catch (e4) {
    res = "exception";
} finally {
    Assert.expectEq("Set package private untyped var", "exception", res);
}


