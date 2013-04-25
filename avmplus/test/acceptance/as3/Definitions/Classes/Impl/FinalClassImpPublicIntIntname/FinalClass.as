/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Final Class FinalClass
 * Class methods
 *
 */

package FinalClassImpPublicIntIntname{

    final class FinalClass implements PublicInt, PublicInt_S{
        
        public function deffunc():String{
            return"PASSED";
        }
        /*PublicInt_S function deffunc():String{
            return"PASSED";
        }*/

    }

public class ClassGet {
    private var obj:FinalClass = new FinalClass();
    var i:PublicInt = obj;
         var i_s:PublicInt_S = obj;

        public function accdeffunc():String{return i.deffunc();}
        public function accdeffunc_s():String{return i_s.deffunc();}
        // access default function deffunc
            
           }
}
