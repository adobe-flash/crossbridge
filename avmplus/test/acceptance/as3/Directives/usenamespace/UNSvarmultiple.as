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
namespace Basketball;
namespace Hockey;

Baseball var teamName = "Giants";
Baseball var teamColor = "green";
Football var teamName = "Chargers";
Basketball var teamName = "Kings";
Hockey var teamName = "Sharks";
Hockey var teamColor = "yellow";

public function a1(){
  use namespace Baseball;
  return Baseball::teamName
}
public function a2(){
  use namespace Football;
  return Football::teamName
}
public function a3(){
  use namespace Basketball;
  return Basketball::teamName
}
public function a4(){
  use namespace Hockey;
  return Hockey::teamName
}
public function a5(){
  use namespace Hockey;
  return Hockey::teamColor
}


}

var obj:A = new A();

Assert.expectEq( "Baseball var teamName = 'Giants'", "Giants", obj.a1());

Assert.expectEq( "Football var teamName = 'Chargers'", "Chargers", obj.a2() );

Assert.expectEq( "Basketball var teamName = 'Kings'", "Kings", obj.a3());

Assert.expectEq( "Hockey var teamName = 'Sharks'", "Sharks", obj.a4() );

Assert.expectEq( "Hockey var teamColor = 'yellow'", "yellow", obj.a5() );

              // displays results.
