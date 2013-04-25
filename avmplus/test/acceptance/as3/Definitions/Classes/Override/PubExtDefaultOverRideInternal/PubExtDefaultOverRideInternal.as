/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClassPackage {

    import DefaultClassPackage.*;

    public class PubExtDefaultOverRideInternal extends DefaultClass {

        public var orSet = false;
        public var orGet = false;

        // *******************************
        // override a public method
        // with a public method
        // *******************************

        override internal function getBoolean(): Boolean {
            orGet = true;
            return this.boolean;
        }

        override internal function setBoolean( b:Boolean ) {
            orSet = true;
            this.boolean = b;
        }
        
        public function setGetBoolean(b:Boolean):Boolean {
            this.setBoolean(b);
            return this.getBoolean();
        }
    }
}
