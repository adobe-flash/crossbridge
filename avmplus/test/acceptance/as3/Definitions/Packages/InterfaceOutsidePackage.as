/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package foo {
    public interface fletch {
        function lives();
    }

}

import com.adobe.test.Assert;
interface fletch {
    function lives();
}

class erwin implements fletch {
    public function lives(){return 'outside of package';}
}

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "123167";



var FLETCHCLASS = new erwin();
Assert.expectEq( "interface outside of package", "outside of package", FLETCHCLASS.lives() );

              // displays results.
