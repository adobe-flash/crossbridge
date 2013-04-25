/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package DefaultClass {

    import DefaultClass.*;

    class ExtDefaultOverRidePublicInner extends DefaultClass {
    
        public var orSetString = "FAILED";
        public var orGetString = "FAILED";

        // *******************************
        // override a public method
        // with a public method
        // *******************************

        override public function getPubString(): String {
            orGetString = "PASSED";
            return this.string+"_override";
        }

        override public function setPubString(s:String) {
                        
                string = "override_"+s;
                        
                    
            orSetString = "PASSED";
        }

    }

    public class ExtDefaultOverRidePublic extends ExtDefaultOverRidePublicInner {}
}
