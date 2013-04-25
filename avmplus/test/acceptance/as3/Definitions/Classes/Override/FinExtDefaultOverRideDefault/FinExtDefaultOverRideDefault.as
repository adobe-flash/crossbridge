/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClass {

    import DefaultClass.*;

    public final class FinExtDefaultOverRideDefault extends DefaultClass {

        public var orSet = "FAILED";
        public var orGet = "FAILED";

        // *********************************
        // override a default method
        // with a default method in subclass
        // *********************************

        override function setNumber(n:Number) {
            orSet = "PASSED";
            this.number = n+10;
        }

        override function getNumber() : Number {
            orGet = "PASSED";
            return number+10;
        }

        // public access method
        public function setGetNumber(n:Number):Number {
            this.setNumber(n);
            return getNumber();
        }

    }

}
