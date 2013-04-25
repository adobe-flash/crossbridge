/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package InternalClassPackage {

    import InternalClassPackage.*;

    dynamic class DynExtInternalOverRidePublicInner extends InternalClass {

        public var orSet = false;
        public var orGet = false;

        // *******************************
        // override a public method
        // with a public method
        // *******************************

        override public function getBoolean(): Boolean {
            orGet = true;
            return this.boolean;
        }

        override public function setBoolean( b:Boolean ) {
            orSet = true;
            this.boolean = b;
        }

    }
    public class DynExtInternalOverRidePublic extends DynExtInternalOverRidePublicInner {}

}
