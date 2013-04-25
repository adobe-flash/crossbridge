/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "namespace attributes";       // Provide ECMA section title or a description
var BUGNUMBER = "";




class A{
public namespace Baseball;

// public namespace
public function getTeam() { return "Public"; }
Baseball function getTeam() { return "Giants";}

public function a1(){
  return Baseball::getTeam();
  }


public function a2(){
use namespace Baseball
  return public::getTeam();
}
public function a3(){
use namespace Baseball
  return Baseball::getTeam();
}

}

var obj:A = new A();

Assert.expectEq( "public::getTeam() - no namespace loaded", "Public", obj.getTeam());
Assert.expectEq( "Baseball::getTeam() - no namespace loaded", "Giants", obj.a1());
Assert.expectEq( "public::getTeam() - after use namespace Baseball", "Public", obj.a2());
Assert.expectEq( "getTeam() - after use namespace Baseball", "Giants", obj.a3());

              // displays results.
