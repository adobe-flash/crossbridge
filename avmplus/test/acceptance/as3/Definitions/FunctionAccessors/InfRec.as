/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "FunctionAccessors";
// var VERSION = "AS3";
// var TITLE   = "Function Accessors";
var BUGNUMBER = "";


class foo{
    // infinite recursion calling setter from setter and back again
    private var _inf;
    private var _inf2;
    public function get inf(){ return inf2; }
    public function get inf2(){ return inf; }

}

var OBJ = new foo();
try{
    var res = "not run";
    OBJ.inf; // should cause infinite recursion
    res = "no exception";
} catch (e) {
    res = "exception";
} finally {
    Assert.expectEq("Infinite recursion getter calling getter", "exception", res);
}


