/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package IntClassImpInternalInt{
namespace ns;

        
    

    internal interface InternalInt{
    
    function pubFunc():Boolean;        //attribute method
        function deffunc2():Number; //interface namespace  method
        function deffunc3():int;     //interface namespace  method
    
    }

    internal class InternalClass implements InternalInt{
        
                public function deffunc2():Number{
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
        
class DefaultsubClass extends InternalClass{
        

        

        
            
        ns override function nsFunc(a="pass"):int{         //Namespace method
            return a.length;
        }
        
    }

    public class DefsubExtIntClassImpIntInt extends DefaultsubClass{
                //var pubsupacc = new DefaultClassAccesor();
                var DefSubClass = new DefaultsubClass();
                var DefSupClass = new InternalClass();
        var i:InternalInt=DefSupClass;
            public function accdeffunc2():Number{return i.deffunc2();}
                public function accdeffunc3():int{return DefSupClass.InternalInt::deffunc3();}
                public function accpubfunc(){return pubFunc();}
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
 
      
    
}
