/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

 /*
  * Use the namespace defined in varsDef.as
  * avmplus varsDef.abc simple.abc
 */

// var SECTION = " ";
// var VERSION = "AS3";
// var TITLE   = "import namespace defined from a varsDef.abc file";


class A{
use namespace Baseball;
use namespace Basketball;

Baseball var teamName = "Giants";
Basketball var teamName = "Kings";
foo var teamName = "Chargers";
}

var obj:A = new A()
Assert.expectEq( "use namespace 'Baseball' ", "Giants", obj.Baseball::teamName);
Assert.expectEq( "use namespace 'Basketball' ", "Kings", obj.Basketball::teamName);
Assert.expectEq( "use namespace 'Basketball' ", "Chargers", obj.foo::teamName);



