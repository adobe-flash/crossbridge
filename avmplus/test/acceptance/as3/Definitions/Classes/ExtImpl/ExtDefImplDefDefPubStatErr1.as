/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


class DefaultClass {
  public static function baseClass() {
     // print("base is invoked");
  }
}

// var SECTION = "RTE";
// var VERSION = "AS3";
// var TITLE = "accessing public static method outside of the class";
var BUGNUMBER = "";


var out = new DefaultClass();

var error = "no error thrown";

try {
  out.baseClass();
} catch (e) {
  error = e.toString();
}

Assert.expectEq("accessing public static method outside of the class, RTE #1069",
    "ReferenceError: Error #1069",
    error.substr(0,27));
    // print( "FAILED, should give run time error : accessing public static method outside of the class" );


