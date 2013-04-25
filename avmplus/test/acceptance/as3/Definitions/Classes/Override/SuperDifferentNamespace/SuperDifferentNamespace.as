/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package SuperDifferentNamespace {
    public class SuperDifferentNamespace extends BaseClass {
    
        override ns1 function echo() : String {
            return super.ns2::echo() + " override";
        }
        
    }
}
