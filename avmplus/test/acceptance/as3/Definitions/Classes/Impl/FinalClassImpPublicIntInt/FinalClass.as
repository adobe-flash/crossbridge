/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Final Class FinalClass
 * Class methods
 *
 */

package FinalClassImpPublicIntInt{

    final class FinalClass implements PublicInt, PublicInt_S{
        
        public function deffunc():String{
            return"PASSED";
        }

    }

public class ClassGet {
    private var obj:FinalClass = new FinalClass();

        public function accdeffunc():String{return obj.deffunc();}
        // access default function deffunc
            
       }
}
