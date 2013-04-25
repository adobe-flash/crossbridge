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
namespace Football;

Baseball var teamName = "Giants";
Football var teamName = "Chargers";

public function a1(){
return teamName
}

use namespace Baseball;

public function getTeam(){
    use namespace Football;
    return Football::teamName;
}

public function a2(){
return teamName
}
}

var obj:A = new A();
// should be using Baseball

Assert.expectEq( "variable ", "Giants", obj.a1());

// inside function will call use namespace Football
Assert.expectEq( "function getTeam called use namespace locally", "Chargers", obj.getTeam() );

// should still be using Baseball
Assert.expectEq( "use namespace outside of function after function call", "Giants", obj.a2() );


              // displays results.
