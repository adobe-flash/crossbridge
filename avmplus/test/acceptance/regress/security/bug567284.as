/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
    import avmplus.Domain
    import avmplus.System
import com.adobe.test.Assert;

//     var SECTION = "Domain";
//     var VERSION = "Domain";
//     var TITLE   = "Bug 567284";


    var support_path:String = System.argv[0];

    var parentDomain:Domain = Domain.currentDomain;
    var childDomain:Domain = new Domain(parentDomain);

    childDomain.load(support_path + "DupExaminer.abc");
    childDomain.load(support_path + "Dup1.abc");

    var cls:Class = childDomain.getClass("DupExaminer");
    var examiner = new cls();
    var value = examiner.examine();

    Assert.expectEq("Test for non-crash in child Domain",
      "Dup1",
      String(value));

    parentDomain.load(support_path + "Dup2.abc")

    value = examiner.examine();

    // Note that we still expect "Dup1" as the first version of "Dup"
    // loaded should still take precedence, even though "Dup2" is higher
    // in the Domain chain, due to freeze-on-first-use
    Assert.expectEq("Test for non-crash in parent Domain",
      "Dup1",
      String(value));


}
