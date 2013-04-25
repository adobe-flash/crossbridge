/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Default Class DefaultClass
 * Class methods
 *
 */

package DefClassImpInternalIntIntname{

    class DefaultClass implements InternalInt, InternalInt_S{
        

        public function deffunc():String{
            return"PASSED";
        }
        
                /*InternalInt_S function deffunc():String{
            return"PASSED";
        }*/
    
    }

    public class DefaultClassAccesor{

                var c:DefaultClass = new DefaultClass();
        public function accdeffunc(){return c.InternalInt::deffunc();}
        public function accdeffunc_s(){return c.InternalInt_S::deffunc();}
            

       }
    
}
