/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package PubClassImpInternalInt{
use namespace ns;
         internal interface InternalInt{
    
        function deffunc():String;                     //Default                             //function deffunc2():Number; //interface namespace  method
                function deffunc3():int;     //interface namespace  method                                                       //attribute method

    
    }

    public class PublicClass implements InternalInt{
        

        public function deffunc():String{                  //Default method
            return"PASSED";
        }
                /*InternalInt function deffunc2():Number{ //Interface Namespace method                  var a:Number;
                        a=10;
            return a;
        }*/
                public function deffunc3():int{ //Interface Namespace method                    var b:int;
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
 class DefaultsubClass extends PublicClass{
        

        override public function deffunc():String{ //Default method
            return "really"+"PASSED";
        }


        override public function pubFunc():Boolean{//Public method
                  

                return !true;
        }
            
        ns override function nsFunc(a="test"):int{         //Namespace method
            return a.length;
        }
        
    }

    public class DefaultsubClassAccessor extends DefaultsubClass{
                //var pubsupacc = new DefaultClassAccesor();
                var DefSubClass = new DefaultsubClass();
                var DefSupClass = new PublicClass();
                var i:InternalInt = DefSupClass;
        public function accdeffunc(){return DefSubClass.deffunc();}
        // access default function deffunc
                //public function accdeffunc2(){return i.deffunc2();}
                public function accdeffunc3(){return DefSupClass.InternalInt::deffunc3();}
                public function accsupdeffunc(){return DefSupClass.deffunc();}
                public function accpubfunc(){return pubFunc();}
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
      
    
}
