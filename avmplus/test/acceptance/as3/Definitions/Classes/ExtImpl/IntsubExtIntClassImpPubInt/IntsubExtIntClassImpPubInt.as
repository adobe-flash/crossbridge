/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package InternalClassImpPublicInt{
namespace ns;


               public interface PublicInt{
    
        
          function pubFunc():Boolean;          //attribute method

//      ns function nsFunc(a="test"):int;          //Namespace attribute method
    }

    internal class InternalClass implements PublicInt{
        
        
        public function pubFunc():Boolean{         //Public method
                return true;
        }
            
        ns function nsFunc(a="test"):int{          //Namespace method
            return a.length;
        }
        
    }


    
       class InternalsubClass extends InternalClass{
        

        


        
            
        ns override function nsFunc(a="test"):int{         //Namespace method
            return a.length;
        }
        
    }

    public class IntsubExtIntClassImpPubInt extends InternalsubClass{

                
            
        public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
        
    
}
