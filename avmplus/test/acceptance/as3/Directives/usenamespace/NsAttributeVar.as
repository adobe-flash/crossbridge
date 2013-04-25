/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package P {
 
     public namespace nsPublic1
     public namespace nsPublic2
     internal namespace nsInternal1
     internal namespace nsInternal2
     

    public class Game{

 protected namespace nsProtected1
  protected namespace nsProtected2
  private namespace nsPrivate1
     private namespace nsPrivate2
        
             nsPublic1 var x:int = 10;
             nsPublic2 var x:String = "team1";
             nsInternal1 var y:int = 10;
             nsInternal2 var y:String = "team1";
         nsPrivate1 var z:int = 10;
             nsPrivate2 var z:String = "team1";
             nsProtected1 var a:int = 10;
             nsProtected2 var a:String = "team1";
        
             public function accintvar1(){return nsInternal1::y} //public function to access internal variable
             public function accintvar2(){return nsInternal2::y} //public function to access internal variable
             public function accprivvar1(){return nsPrivate1::z} //public function to access private variable
             public function accprivvar2(){return nsPrivate2::z} //public function to access private variable
             public function accprotvar1(){return nsProtected1::a} //public function to access protected variable
             public function accprotvar2(){return nsProtected2::a} //public function to access protected variable

     }
 }

 
// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "Public_Private_Internal_Protected_Namespace as variable attribute";       // Provide ECMA section title or a description
var BUGNUMBER = "";






import P.*;

import com.adobe.test.Assert;
var game = new Game();



Assert.expectEq( "Public Namespace as variable attribute x = 10", 10, game.nsPublic1::x);
Assert.expectEq( "Public Namespace as variable attribute x = 'team1'", "team1", game.nsPublic2::x);
Assert.expectEq( "Internal Namespace as variable attribute y = 10", 10, game.accintvar1());
Assert.expectEq( "Internal Namespace as variable attribute y = 'team1'", "team1", game.accintvar2());
Assert.expectEq( "Private Namespace as variable attribute z = 10", 10, game.accprivvar1());
Assert.expectEq( "Private Namespace as variable attribute z = 'team1'", "team1", game.accprivvar2());
Assert.expectEq( "Protected Namespace as variable attribute a = 10", 10, game.accprotvar1());
Assert.expectEq( "Protected Namespace as variable attribute a = 'team1'", "team1", game.accprotvar2());

              // displays results.
