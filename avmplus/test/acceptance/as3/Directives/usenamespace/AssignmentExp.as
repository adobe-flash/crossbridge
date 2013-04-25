/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "namespace assigned with AssignmentExpression";       // Provide ECMA section title or a description
var BUGNUMBER = "";




class A{

    public namespace N1;
    public namespace N2;
    public namespace N3 = "foo";
    public namespace N4 = N3;

    var v1:int =1;
    var v2:int =2;
    var v3:int =5;
    N1 var v3;
    N2 var n2;
    N2 var n3
    N3 var v1 = 5;

    function a1() {
         N1::v3=v1+v2;
         return N1::v3;
    }
    function a2() {
         N2::n2 = a1();
         return N2::n2;
    }
    function a3() {
         N2::n2 = v3;
         return N2::n2;
    }
    function a4() {
          return N4::v1;
    }
    function a5() {
        try {
            N1::v1=5;
            result = "no exception";
        } catch(e3) {
            result = e3.toString();
        }
        return result;
    }
    function a6() {
        N1::['v3']=4;
        return N1::['v3'];
    }
}

class C
{
    mx_internal var v:int = 0;
    mx_internal static var sv:int = 0;
}


var obj:A = new A();

 Assert.expectEq("N1::v3=3", 3, obj.a1());
 Assert.expectEq("N1::['v3']=4", 4, obj.a6());
 Assert.expectEq("N2::n2='3'", 3, obj.a2());
 Assert.expectEq("v3='5'", 5, obj.v3);
 Assert.expectEq("N2::n2='5'", 5, obj.a3());
 Assert.expectEq("N4 = N3; N4::v1", 5, obj.a4());
 
 namespace mx_internal;
 
use namespace mx_internal;
 
 var c:C = new C();
 
 try {
    Assert.expectEq("c.v++", 1, (c.v++, c.v));
    result = "no exception";
 } catch (e1) {
    result = "exception";
 }
 Assert.expectEq("Increment in setter", "no exception", result);
 
 try {
    Assert.expectEq("C.sv++", 1, (C.sv++, C.sv));
    result = "no exception";
 } catch (e2) {
    result = "exception";
 }
 Assert.expectEq("Increment in static setter", "no exception", result);
 
 Assert.expectEq("N1::v1=5 without declaring N2 var v1", "ReferenceError: Error #1056", Utils.referenceError(obj.a5()));

