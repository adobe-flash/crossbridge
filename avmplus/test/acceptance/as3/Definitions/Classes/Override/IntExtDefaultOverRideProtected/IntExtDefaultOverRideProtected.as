/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package DefaultClassPackage {
    internal class IntExtDefaultOverRideProtectedInner extends DefaultClass {
        public var orSet:Boolean = false;
        public var orGet:Boolean = false;


        // *****************
        // protected methods
        // *****************
        override protected function getDate() : Date {
            orGet = true;
            return date;
        }
        override protected function setDate( d:Date ) {
            orSet = true;
            date = d;
        }
        
        public function setGetDate( d:Date ) : Date {
            setDate(d);
            return getDate();
        }

    }
    
    public class IntExtDefaultOverRideProtected extends IntExtDefaultOverRideProtectedInner {}

}
