/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package MethodInNamespace {
    public namespace ns1;
    public namespace ns2;
    
    public class BaseClass {
        public var i:int;

        ns1 function getI() : int {
            return i;
        }
        
        ns2 function getI() : int {
            return i + 2;
        }
    }
    
}
