/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "use namespace";       // Provide ECMA section title or a description
var BUGNUMBER = "";





class A{

namespace Baseball;

Baseball var teamName = "Giants";

public function a1(){
use namespace Baseball;
return teamName
}
}

var obj:A = new A()

Assert.expectEq( "Baseball var teamName = 'Giants'", "Giants", obj.a1() );


              // displays results.
