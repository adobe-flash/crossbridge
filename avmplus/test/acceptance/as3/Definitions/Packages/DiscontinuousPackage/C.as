/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package DiscontinuousPackage {
    use namespace Kitty;
    
    public class C {
        Kitty var b:Boolean = true;
        public function returnNSVar():Boolean {
            return Kitty::b;
        }
        
        Kitty function returnArray():Array {
            return [1,2,3];
        }
        
        public function callNSFunc() {
            var k:Array = Kitty::returnArray();
            return k;
        }
    }
}
