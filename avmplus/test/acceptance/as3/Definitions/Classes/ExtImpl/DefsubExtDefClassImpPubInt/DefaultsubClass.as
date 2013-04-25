/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package DefClassImpPublicInt{
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

                var DefSubClass = new DefaultsubClass();
                var DefSupClass = new DefaultClass();
                var i:PublicInt = DefSupClass;
        public function accdeffunc(){return DefSubClass.deffunc();}
        // access default function deffunc
                public function accdeffunc2(){return i.deffunc2();}
                public function accdeffunc3(){return DefSupClass.PublicInt::deffunc3();}
                public function accsupdeffunc(){return DefSupClass.deffunc();}
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
    
}
