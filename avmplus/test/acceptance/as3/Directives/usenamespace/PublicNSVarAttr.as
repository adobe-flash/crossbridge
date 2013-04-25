/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "namespace attributes";       // Provide ECMA section title or a description
var BUGNUMBER = "";




class A{
namespace Baseball;

// public namespace
public var teamName="Public";
Baseball var teamName="Giants"

public function a1(){
 return public::teamName
}

public function a2(){
 return Baseball::teamName
}

public function a3(){
use namespace Baseball;
 return public::teamName
}

public function a4(){
use namespace Baseball;
 return Baseball::teamName
}
}

var obj:A = new A();

Assert.expectEq( "public::teamName", "Public", obj.a1());
Assert.expectEq( "Baseball::teamName", "Giants", obj.a2());

Assert.expectEq( "public::teamName", "Public", obj.a3());
Assert.expectEq( "Baseball::teamName", "Giants", obj.a4());
              // displays results.
