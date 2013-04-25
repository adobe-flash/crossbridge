/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClass {

    import DefaultClass.*;

    dynamic class DynExtDefaultOverRideVirtualInner extends DefaultClass {
    
        public var orSet = false;
        public var orGet = false;

        // *********************************
        // override a virtual method
        // with a default method in subclass
        // *********************************

        override virtual function setVirtualString( newString:String ) {
            orSet = true;
            string = "override: " + newString;
        }

        override virtual function getVirtualString() : String{
            orGet = true;
            return string;
        }

        // public accessor to work outside of package
        public function setGetString( newString:String ) : String {
            setVirtualString(newString);
            return getVirtualString();
        }
        
        public function callSuper( input:String ) : String {
            super.setVirtualString(input);
            return super.getVirtualString();
        }

        

    }

    public class DynExtDefaultOverRideVirtual extends DynExtDefaultOverRideVirtualInner {}

    
}
