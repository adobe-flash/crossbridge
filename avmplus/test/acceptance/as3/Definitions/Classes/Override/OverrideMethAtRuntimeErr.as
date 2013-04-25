/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
class BaseClass {
  public function foo(){}
}

dynamic class OverrideMethAtRuntimeErr extends BaseClass {}

// var SECTION = "test";
// var VERSION = "AS3";
// var TITLE = "Override Method at runtime error";
var BUGNUMBER = "";


var obj:OverrideMethAtRuntimeErr = new OverrideMethAtRuntimeErr;

var error = "no error thrown";

try {
  obj.foo = function () { return "here"; }
} catch (e) {
  error = e.toString();
}

Assert.expectEq("Override Method At Runtime",
  "ReferenceError: Error #1037",
  error.substr(0,27));


