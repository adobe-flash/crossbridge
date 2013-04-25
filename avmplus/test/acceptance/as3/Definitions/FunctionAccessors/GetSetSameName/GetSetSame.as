/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetSameName {

    public class GetSetSame {

          
        private var _x:Array = [1,2,3];

        public function get GetSetSameName():Array {
            return _x;
        }
        public function set GetSetSameName( a:Array ) {
            _x=a;
        }
        
    }

}
