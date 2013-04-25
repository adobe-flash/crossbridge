/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Final Class FinalClass
 * Class methods
 *
 */

package FinalClassImpDefIntIntname{

    final class FinalClass implements DefaultInt, DefaultInt_S{
        
        public function deffunc():String{
            return"PASSED";
        }
        /*DefaultInt_S function deffunc():String{
            return"PASSED";
        }*/

    }

public class ClassGet {

    private var obj:FinalClass = new FinalClass();
    var i:DefaultInt = obj;
    var i_s:DefaultInt_S = obj;
    public function accdeffunc(){return i.deffunc();}
    public function accdeffunc_s(){return i_s.deffunc();}
        
       }
}
