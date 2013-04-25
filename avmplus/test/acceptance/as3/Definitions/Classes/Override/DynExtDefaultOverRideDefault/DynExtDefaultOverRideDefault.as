/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClass {

    import DefaultClass.*;

    dynamic class DynExtDefaultOverRideDefaultInner extends DefaultClass {

        // *********************************
        // override a default method
        // with a default method in subclass
        // *********************************

        override function setArray( newArray:Array) {
            a = ["override","worked"];
            a.push(newArray);
        }

        override function getArray() : Array{
            return a;
        }

        // public accessor to work outside of package
        public function setGetArray(arr:Array) : Array {
            setArray(arr);
            return getArray();
        }

        

    }

    public class DynExtDefaultOverRideDefault extends DynExtDefaultOverRideDefaultInner {}

    
}
