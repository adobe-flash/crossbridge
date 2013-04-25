/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Dynamic Class DynamicClass
 * Class methods
 *
 */

package DynamicClassImpPublicInt{

    dynamic class DynamicClass implements PublicInt{
        

        public function deffunc():String{
            return"PASSED";
        }

    
    }

    public class DynamicClassAccessor{

           var acc:DynamicClass;

        public function DynamicClassAccessor(){
             acc = new DynamicClass();
        }

        public function accdeffunc():String{return acc.deffunc();}
                    
       }
}
