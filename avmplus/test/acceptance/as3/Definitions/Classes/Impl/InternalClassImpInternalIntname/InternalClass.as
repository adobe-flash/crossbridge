/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Internal Class InternalClass
 * Class methods
 *
 */

package InternalClassImpInternalIntname{


    internal class InternalClass implements InternalInt{
        
        public function deffunc():String{
            return"PASSED";
        }
        
    }


    public class InternalClassAccesor extends InternalClass{
    
        
        var c:InternalClass = new InternalClass();
        var i:InternalInt = c;
        public function accdeffunc(){return i.deffunc();}
            
      }
    
}
