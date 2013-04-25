/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package DefaultClassPackage {
    class DefaultClass {
        var date:Date;

        // ****************
        // constructor
        // ****************
        function DefaultClass() {}

        // *****************
        // protected methods
        // *****************
        protected function getDate() : Date { return date; }
        protected function setDate( d:Date ) { date = d; }

    }

}
