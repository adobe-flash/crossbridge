/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package GetterSetter {

    public class BaseClass {
        private var _string:String;
        
        public function get string() : String { return _string; }
        public function set string( s:String ) { _string=s; }
    
    }


}
