/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package GetSetSameName {

    public class GetSetSameName {

          
        private static var _x:Array = [1,2,3];

        public static function get x():Array {
            return _x;
        }
        public static function set x( a:Array ) {
            _x=a;
        }
        
    }

}
