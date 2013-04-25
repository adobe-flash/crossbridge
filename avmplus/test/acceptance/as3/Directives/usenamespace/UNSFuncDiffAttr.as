/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "namespace used in the case of same function is defined multiple defined";
var BUGNUMBER = "";




namespace Football;
class Game{

    public namespace Baseball;
    public namespace Football;

    public function getTeam(){
        teamName = "Dummy";
            return teamName;
    }

    Football function getTeam(){
        var teamName = "Chargers";
             return teamName;
    }

   Baseball function getTeam(){
        var teamName = "Giants";
            return teamName;
   }

   public function accgetTeam1(){
    return Football::getTeam()
   }
   public function accgetTeam2(){
    return Baseball::getTeam()
   }
}

var obj:Game = new Game();

Assert.expectEq( "function getTeam called", "Dummy", obj.getTeam());
Assert.expectEq( "function getTeam called", "Chargers", obj.accgetTeam1() );
Assert.expectEq( "function getTeam called", "Giants", obj.accgetTeam2() );



              // displays results.
