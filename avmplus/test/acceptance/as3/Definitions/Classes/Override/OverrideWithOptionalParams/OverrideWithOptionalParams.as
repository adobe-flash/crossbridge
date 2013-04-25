/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package OptionalParams {
    
    public class OverrideWithOptionalParams extends BaseClass {

        override public function setInt( input:int = 1 ) {
            i = input;
        }
        
        override public function setString( string:String = "override" ) {
            s = string;
        }
        
        override public function setAll( input:int = 1, string:String = "override" ) {
            i = input;
            s = string;
        }
        
    }
    
}
