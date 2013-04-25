/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package PublicClassImpPublicInt{
use namespace ns;

   public class PublicsubClass extends PublicClass{
        

    //override public function getdeffunc():String{return deffunc();}
        // access default function deffunc


    override public function pubFunc():Boolean{        //Public method
                return !true;
        }
            
    override ns function nsFunc(a="test"):int{         //Namespace method
            return a.length;
        }
        
    }

    
    
}
