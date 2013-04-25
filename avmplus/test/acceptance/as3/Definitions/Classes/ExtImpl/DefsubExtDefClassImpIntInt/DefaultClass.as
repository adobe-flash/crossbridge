/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DefClassImpInternalInt{
use namespace ns;

    class DefaultClass implements InternalInt{
        

        public function deffunc():String{                 //Default method
            return"PASSED";
        }
                public function deffunc2():Number{ //
                    var a:Number;
                        a=10;
            return a;
        }
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

    public class DefaultClassAccessor{

                var B = new DefaultClass();
                var i:InternalInt=B;
        public function accdeffunc(){return deffunc();}
        // access default function deffunc
                public function accdeffunc2():Number{return i.deffunc2();}
                public function accdeffunc3():int{return B.InternalInt::deffunc3();}
                
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
    
}
