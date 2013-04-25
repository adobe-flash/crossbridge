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
namespace Basketball;
namespace Hockey;

Baseball var teamName = "Giants";
Basketball var teamName = "Kings";
Hockey var teamName = "Sharks";

public function getTeam1(){
    use namespace Baseball;
    return Baseball::teamName;
}

public function getTeam2(){
    use namespace Basketball;
    return Basketball::teamName;
}

public function getTeam3(){
    use namespace Hockey;
    return Hockey::teamName;
}
}
var obj:A = new A();

Assert.expectEq( "function getTeam1(){use namespace Baseball}", "Giants", obj.getTeam1() );
Assert.expectEq( "function getTeam2(){use namespace Basketball}", "Kings", obj.getTeam2() );
Assert.expectEq( "function getTeam3(){use namespace Hockey}", "Sharks", obj.getTeam3() );


              // displays results.
