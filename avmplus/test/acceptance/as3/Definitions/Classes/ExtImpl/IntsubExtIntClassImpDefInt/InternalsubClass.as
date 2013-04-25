/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package InternalClassImpDefInt{
use namespace ns;

    class InternalsubClass extends InternalClass{
        

        // function deffunc():String{ //Default method
        //  return "really"+"PASSED";
        //}


        override public function pubFunc():Boolean{//Public method
                  

                return !true;
        }
            
        ns override function nsFunc(a="test"):int{         //Namespace method
            return a.length;
        }
        
    }

    public class InternalsubClassAccessor extends InternalsubClass{

                var IntSubClass = new InternalsubClass();
                var IntSupClass = new InternalClass();
        //public function accdeffunc(){return IntSubClass.deffunc();}
        // access default function deffunc
                //public function accsupdeffunc(){return IntSupClass.deffunc();}
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
    
}
