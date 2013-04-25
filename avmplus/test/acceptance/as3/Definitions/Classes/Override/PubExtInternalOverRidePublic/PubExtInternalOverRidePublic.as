/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package InternalClassPackage {

    import InternalClassPackage.*;

    public class PubExtInternalOverRidePublic extends InternalClass {

        // *********************************
        // override a internal method
        // *********************************
        override public function setArray( a:Array ) {
            array = ["override","worked"];
            array.push(a);
        }

        override public function getArray() : Array{
            return array;
        }

        

    }
    
}
