/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package PublicClass {

    import PublicClass.*;

    public class PubExtPublicOverRidePublic extends PublicClass {

        public var orSet = false;
        public var orGet = false;

        // *******************************
        // override a public method
        // with a public method
        // *******************************

        override public function getPubBoolean(): Boolean {
            orGet = true;
            return this.boolean;
        }

        override public function setPubBoolean( b:Boolean ) {
            orSet = true;
            this.boolean = b;
        }

        public function callSuper(b:Boolean):Boolean {
            super.setPubBoolean(b);
            return super.getPubBoolean();
        }
    }
}
