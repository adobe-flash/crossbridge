/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package P {
 
    public namespace Baseball;
 
    public class Game{
         Baseball var teamName = "Giants";
         use namespace Baseball;
     }
  }

 
 


// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "namespace inside package";       // Provide ECMA section title or a description
var BUGNUMBER = "";





import P.*;
var game = new Game();
import com.adobe.test.Assert;

Assert.expectEq( "Baseball var teamName = 'Giants'", "Giants", game.Baseball::teamName );

var teamName = "Test"

Assert.expectEq( "Baseball var teamName = 'Test'", "Test", teamName );



              // displays results.
