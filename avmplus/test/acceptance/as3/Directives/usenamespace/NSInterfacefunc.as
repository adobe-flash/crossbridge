/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "Interface defined in a namespace";       // Provide ECMA section title or a description
var BUGNUMBER = "";




namespace English   // define public::English
namespace French        // define public::French
//use namespace French  // add public::French to the open namespaces


interface IPod {
    function playMusic()
    
    function rewind()
        
        function forward()
    
}

class MP3Player implements IPod {

    public function playMusic() {
        return "Dead Souls";
    }
    public function rewind() {
        return "Monday Paracetamol";
    }
    public function forward() {
        return "Dead Cities";
    }
    
}

var p = new MP3Player();

Assert.expectEq("Public function in interface", "Dead Souls", p.playMusic() );
Assert.expectEq("Namespace function in interface", "Dead Cities", p.IPod::forward() );

// NOTE //
// When this is running, add *Err test case to make sure you get the error message for
// not defining a function included in the namespace interface.
//////////

