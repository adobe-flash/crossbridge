/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package OptionalParams {
    
    public class BaseClass {
        public var i:int;
        public var s:String;

        public function setInt( input:int = 0 ) {
            i = input;
        }
        
        public function setString( string:String = "default" ) {
            s = string;
        }
        
        public function setAll( input:int = 0, string:String = "default" ) {
            i = input;
            s = string;
        }
        
    }
    
}
