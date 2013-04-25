/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package {

import com.adobe.test.Assert;
// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "Qualified expression reference";       // Provide ECMA section title or a description


public namespace Football
public namespace Baseball

public class Game {
    Football var n =  "Joe"
    Baseball var n =  "Steve"
}


var g = new Game();

Assert.expectEq( "Football::['n']", "Joe", g.Football::['n']);

Assert.expectEq( "Baseball::['n']", "Steve", g.Baseball::['n']);

}

              // displays results.
