/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package PublicClassPackage {
    public class PublicClass {
        
        var date:Date;
        
        // ****************
        // constructor
        // ****************
        function PublicClass() {
        }
        
        // *****************
        // internal methods
        // *****************
        internal function getDate() : Date { return date; }
        internal function setDate( d:Date ) { date = d; }
    }

}
