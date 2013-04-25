/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package MCPackage {
    public namespace ns1;
    
    public class GetMCFromNamespace {
        var x:int = 12;
        
        ns1 function getX() : int {
            return this.x;
        }
    }
}
