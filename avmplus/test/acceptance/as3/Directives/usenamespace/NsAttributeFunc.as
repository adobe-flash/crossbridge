/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package P {
 
     public namespace nsPublic
     
     internal namespace nsInternal
     
        

    public class Game{

           protected namespace nsProtected
           private namespace nsPrivate
             nsPublic function whoplay(){return "Steve"}  //public namespace
         nsPrivate function whoplay(){return "Joe"}   //private namespace
             nsInternal function whoplay(){return "Luis"}   //internal namespace
         nsProtected function whoplay(){return "Peter"}   //protected namespace

             public function accprivwhoplay(){return nsPrivate::whoplay()} //public function to access nsPrivate function
             public function accintwhoplay(){return nsInternal::whoplay()} //public function to access nsInternal function
             public function accprotwhoplay(){return nsProtected::whoplay()} //public function to access nsProtected function
        
     }
 }

 
// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "Public_Private_Internal_Protected_Namespace as function attribute";       // Provide ECMA section title or a description
var BUGNUMBER = "";






import P.*;

import com.adobe.test.Assert;
var game = new Game();

Assert.expectEq( "(Public Namespace function) Who is going to play = 'Steve'", "Steve", game.nsPublic::whoplay());
Assert.expectEq( "(Private Namespace function) Who is going to play = 'Joe'", "Joe", game.accprivwhoplay());
Assert.expectEq( "(Internal Namespace function) Who is going to play = 'Luis'", "Luis", game.accintwhoplay());
Assert.expectEq( "(Protected Namespace function) Who is going to play = 'Peter'", "Peter", game.accprotwhoplay());


              // displays results.
