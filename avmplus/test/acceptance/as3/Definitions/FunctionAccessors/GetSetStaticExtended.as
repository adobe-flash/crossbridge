/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import GetSetStaticExtended.*;
import com.adobe.test.Assert;

// var SECTION = "FunctionAccessors";
// var VERSION = "AS3";
// var TITLE   = "Function Accessors";
var BUGNUMBER = "";


var res;

/*
 *
 * statics are not inherited
 * New instances of vars will be created since the statics don't exist
 *
 */

try {
    Assert.expectEq("Static getter var:int", undefined, GetSetStatic.y);
    res  = "no exception";
} catch(e1) {
    res = "exception";
}
Assert.expectEq("Static getter var:int", "no exception", res);

try {
    Assert.expectEq("Static setter var:int", 23334, (GetSetStatic.y = 23334, GetSetStatic.y));
    res  = "no exception";
    } catch(e2) {
        res = "exception";
    }
Assert.expectEq("Static setter var:int", "no exception", res);

try {
    // exception calling toString() on undefined GetSetStatic.x
    GetSetStatic.x.toString();
    res  = "no exception";
    } catch(e3) {
        res = "exception";
    }
Assert.expectEq("Static getter var:Array", "exception", res);

try {
    Assert.expectEq("Static setter var:Array", "4,5,6", (GetSetStatic.x = new Array(4,5,6), GetSetStatic.x.toString()));
    res  = "no exception";
} catch(e4) {
    res = "exception";
}
Assert.expectEq("Static setter var:Array", "no exception", res);

try {
    Assert.expectEq("Static getter var:Boolean", undefined, GetSetStatic.boolean);
    res  = "no exception";
} catch(e5) {
    res = "exception";
}
Assert.expectEq("Static getter var:Boolean", "no exception", res);

try {
    Assert.expectEq("Static setter var:Boolean", false, (GetSetStatic.boolean = false, GetSetStatic.boolean));
    res  = "no exception";
    } catch(e6) {
        res = "exception";
    }
Assert.expectEq("Static setter var:Boolean", "no exception", res);

try {
    Assert.expectEq("Static getter var:uint", undefined, GetSetStatic.u);
    res  = "no exception";
    } catch(e7) {
        res = "exception";
    }
Assert.expectEq("Static getter var:uint", "no exception", res);
try {
    Assert.expectEq("Static setter var:uint", 42, (GetSetStatic.u = 42, GetSetStatic.u));
    res  = "no exception";
    } catch(e8) {
        res = "exception";
    }
Assert.expectEq("Static setter var:uint", "no exception", res);

try {
    Assert.expectEq("Static getter var:String", undefined, GetSetStatic.string);
    res  = "no exception";
    } catch(e9) {
        res = "exception";
    }
Assert.expectEq("Static getter var:String", "no exception", res);

try {
    Assert.expectEq("Static setter var:String", "new string", (GetSetStatic.string = "new string", GetSetStatic.string));
    res  = "no exception";
    } catch(e10) {
        res = "exception";
    }
Assert.expectEq("Static setter var:String", "no exception", res);


