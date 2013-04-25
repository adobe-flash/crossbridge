/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package P {
 
     public namespace Football
     public namespace Baseball

     public class Game{
          
      use namespace Football;
          Football function whoplay(){return "Joe"}
          use namespace Baseball;
          Baseball function whoplay(){return "Steve"}
     }
 }

 
// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "use namespace";       // Provide ECMA section title or a description
var BUGNUMBER = "";






import P.*;

import com.adobe.test.Assert;
var game = new Game();

Assert.expectEq( "Who is going to play = 'Joe'", "Joe", game.Football::whoplay());
Assert.expectEq( "Who is going to play = 'Steve'", "Steve", game.Baseball::whoplay());



              // displays results.
