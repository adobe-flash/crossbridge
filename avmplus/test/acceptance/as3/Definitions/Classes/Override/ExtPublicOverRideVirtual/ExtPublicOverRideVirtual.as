/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package PublicClass {

    import PublicClass.*;

    class ExtPublicOverRideVirtualInner extends PublicClass {

        public var orSet = false;
        public var orGet = false;

        // *******************************
        // override a public method
        // with a public method
        // *******************************

        override virtual function getVirtualBoolean(): Boolean {
            orGet = true;
            return this.boolean;
        }

        override virtual function setVirtualBoolean( b:Boolean ) {
            orSet = true;
            this.boolean = b;
        }
        
        public function getSetVirtualBoolean( b:Boolean ) : Boolean {
            setVirtualBoolean(b);
            return getVirtualBoolean();
        }

        public function callSuper(b:Boolean):Boolean {
            super.setVirtualBoolean(b);
            return super.getVirtualBoolean();
        }
    }

    public class ExtPublicOverRideVirtual extends ExtPublicOverRideVirtualInner {}
}
