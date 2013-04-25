/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Default Class DefaultClass
 * Class methods
 *
 */

package DefClassImpDefIntname{

    class DefaultClass implements DefaultInt{
        

        public function deffunc():String{
            return"PASSED";
        }

        
    }

    public class DefaultClassAccesor{
 
        var c:DefaultClass = new DefaultClass();
        var i:DefaultInt = c;
        public function accdeffunc(){return i.deffunc();}
        // access default function deffunc
            

       }
    
}
