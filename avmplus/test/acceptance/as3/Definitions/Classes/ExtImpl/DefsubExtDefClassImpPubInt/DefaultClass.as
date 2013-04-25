/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DefClassImpPublicInt{
use namespace ns;

    class DefaultClass implements PublicInt{
        

        public function deffunc():String{                  //Default method
            return"PASSED";
        }
                /*PublicInt  function deffunc2():Number{
                    var a:Number;
                        a=10;
            return a;
        }*/
                public function deffunc3():int{
                    var b:int;
                        b=20;
            return b;
        }

        public function pubFunc():Boolean{         //Public method
                return true;
        }
            
        ns function nsFunc(a="test"):int{          //Namespace method
            return a.length;
        }
        
    }

    
    
}
