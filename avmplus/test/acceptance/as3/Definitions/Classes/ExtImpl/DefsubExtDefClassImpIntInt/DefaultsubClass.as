/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DefClassImpInternalInt{
use namespace ns;

    class DefaultsubClass extends DefaultClass{
        

        


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
                var DefSupClass = new DefaultClass();
                //var i:DefaultInt=DefSupClass;
                var k = new DefaultClassAccessor();
        public function accdeffunc(){return DefSubClass.deffunc();}
        // access default function deffunc
                public function retaccdeffunc2(){return k.accdeffunc2();}
                public function retaccdeffunc3(){return k.accdeffunc3();}
                public function accsupdeffunc(){return DefSupClass.deffunc();}
                public function accpubfunc(){return pubFunc();}
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
    
}
