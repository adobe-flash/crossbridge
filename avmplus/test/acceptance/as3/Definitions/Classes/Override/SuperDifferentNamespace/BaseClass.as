/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package SuperDifferentNamespace {
    public namespace ns1;
    public namespace ns2;
    
    public class BaseClass {
        ns1 function echo() : String { return "echo() from BaseClass.ns1"; }
        
        ns2 function echo() : String { return "echo() from BaseClass.ns2"; }
    }
}
