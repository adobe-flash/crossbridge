/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package GetterSetter {

    public class OverrideGetterSetter extends BaseClass {
        public var orGet:Boolean = false;
        public var orSet:Boolean = false;
        
        override public function get string() : String {
            orGet = true;
            return _string;
        }
        
        override public function set string( s:String ) {
            orSet = true;
            _string=s;
        }
        
    
    }


}
