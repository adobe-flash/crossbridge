/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "namespace inside try catch block";       // Provide ECMA section title or a description
var BUGNUMBER = "";





try{

class A{
       namespace N2;
       namespace N3;
       namespace N4;
       
            
       N2 function func() {
           return "Called N2::func1";
       }
       
       N3 function func() {
           return "Called N3::func1";
       }
       public function accfunc1(){
        return N2::func()
       }
       public function accfunc2(){
      return N3::func()
   }
 }

       var obj:A = new A();

       Assert.expectEq( "Calling N2::func1()", "Called N2::func1", obj.accfunc1());
       Assert.expectEq( "Calling N3::func1()", "Called N3::func1", obj.accfunc2());
       
  
} catch (e) {
      Assert.expectEq( "Make sure there is no error", false, true);
}
