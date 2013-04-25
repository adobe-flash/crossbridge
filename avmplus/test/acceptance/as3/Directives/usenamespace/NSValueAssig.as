/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;



// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "Namespace values";       // Provide ECMA section title or a description
var BUGNUMBER = "";






class A{
namespace N1="www.ecma-international.org"
//namespace N2 = N1
namespace N3="flowershop.com";

var a=0;
N3 var flower1:String = "Gerbera Daisy";
namespace N4 = "flowershop.com";
N4 var flower2:String = "Rose";


function a1(){
        if(a!=1) { return N1.toString()}
    else {return null}
}
function a2(){
        return N3::flower1
}
function a3(){
        return N4::flower2
}

}

var obj:A = new A();
Assert.expectEq("Namespacevalue", "www.ecma-international.org", obj.a1());
Assert.expectEq("Getting variable", "Gerbera Daisy", obj.a2());
Assert.expectEq("Getting variable", "Rose", obj.a3());

