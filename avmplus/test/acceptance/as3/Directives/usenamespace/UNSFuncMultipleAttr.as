/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "multiple declaration namespaces in and outside of function";
var BUGNUMBER = "";




class A{
    namespace Baseball;
    namespace Football;
    namespace Basketball;
    namespace Public;

    Football var teamName = "Chargers";

    use namespace Baseball;

    Baseball var teamColor = "blue";
    Basketball var teamName = "basketball";
    Football var teamColor = "yellow";

    Football function getTeam(){
        use namespace Football;
        return teamName;
    }

    public function a1(){
        return Football::getTeam();
    }

    public function a2(){
        return teamColor ;
    }
}

var obj:A = new A()

Assert.expectEq( "function getTeam called use namespace locally", "Chargers", obj.a1());

Assert.expectEq( "Property teamColor called use namespace locally", "blue", obj.a2() );



              // displays results.
