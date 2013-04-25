/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClass {

    import DefaultClass.*;

    class ExtDefaultOverRideInternalInner extends DefaultClass {

        // *********************************
        // override a internal method
        // *********************************
        override internal function setArray( a:Array ) {
            array = ["override","worked"];
            array.push(a);
        }

        override internal function getArray() : Array{
            return array;
        }

        // public accessor to work outside of package
        public function setGetArray(arr:Array) : Array {
            setArray(arr);
            return getArray();
        }

        

    }

    public class ExtDefaultOverRideInternal extends ExtDefaultOverRideInternalInner {}

    
}
