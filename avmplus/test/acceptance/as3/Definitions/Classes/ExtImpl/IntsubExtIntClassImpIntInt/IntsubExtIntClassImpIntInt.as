/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package InternalClassImpInternalInt{

        namespace ns;
    

    internal interface InternalInt{
    
        

     function pubFunc():Boolean;           // attribute method

//      ns function nsFunc(a="test"):int;          //Namespace attribute method
    }

   

    internal class InternalClass implements InternalInt{
        
        

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

    public class IntsubExtIntClassImpIntInt extends InternalsubClass{

                public function accnsFunc(a="test"){return ns::nsFunc(a);}
        // access default function nsFunc
       }
      
                                                                      
    
    
}
