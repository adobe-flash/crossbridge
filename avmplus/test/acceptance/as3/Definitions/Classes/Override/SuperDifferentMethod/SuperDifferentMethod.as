/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package SuperDifferentMethod {
    public class SuperDifferentMethod extends BaseClass {
        override public function echo() : String {
            return super.foo();
        }
        
        public function callFoo() : String {
            return foo();
        }
        

    }
}
