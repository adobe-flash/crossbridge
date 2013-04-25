/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package InternalClassImpDefInt{
use namespace ns;

    internal class InternalClass implements DefaultInt{
        
    public function deffunc():String{    //Default method
            return"PASSED";
        }

        public function pubFunc():Boolean{         //Public method
                return true;
        }
            
        ns function nsFunc(a="test"):int{          //Namespace method
            return a.length;
        }
        
    }


    public class InternalClassAccesor extends InternalClass{
    
        public function accdeffunc(){return deffunc();}
        // access default function deffunc
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
    
}
