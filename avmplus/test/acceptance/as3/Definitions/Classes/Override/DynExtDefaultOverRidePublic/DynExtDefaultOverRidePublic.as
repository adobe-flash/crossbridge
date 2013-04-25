/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClass {

    import DefaultClass.*;

    dynamic class DynExtDefaultOverRidePublicInner extends DefaultClass {

        public var orSetCalled = false;
        public var orGetCalled = false;
    

        // *******************************
        // override a public method
        // with a public method
        // *******************************

        override public function setPubBoolean( b:Boolean ) {
            orSetCalled = true;
            boolean = b;
        }

        override public function getPubBoolean(): Boolean {
            orGetCalled = true;
            return this.boolean;
        }

    }

    //public wrapper
    public dynamic class DynExtDefaultOverRidePublic extends DynExtDefaultOverRidePublicInner {}
}
