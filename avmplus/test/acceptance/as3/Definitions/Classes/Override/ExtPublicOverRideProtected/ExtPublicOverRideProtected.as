/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package PublicClassPackage {

    import PublicClassPackage.*;

    class ExtPublicOverRideProtectedInner extends PublicClass {

        public var orSet = false;
        public var orGet = false;

        // *********************************
        // override a protected method
        // *********************************

        override protected function setBoolean( b:Boolean ) {
            orSet = true;
            this.boolean = b;
        }

        override protected function getBoolean() : Boolean {
            orGet = true;
            return this.boolean;
        }

        public function setGetBoolean(b:Boolean):Boolean {
            this.setBoolean(b);
            return this.getBoolean();
        }
        

    }

    public class ExtPublicOverRideProtected extends ExtPublicOverRideProtectedInner {}

    
}
