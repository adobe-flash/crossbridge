/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
    import avmplus.Domain
    import avmplus.System
import com.adobe.test.Assert;

//     var SECTION = "BugCompatibility";
//     var VERSION = "BugCompatibility";
//     var TITLE   = "Bug 535770";


    var leaf_path:String = System.argv[0];
    var test_path:String = System.argv[1];

    var SWF10_domain = new Domain(Domain.currentDomain);
    SWF10_domain.load(test_path, 10);
    var SWF10_cl = SWF10_domain.getClass("testclass");
    var SWF10_helper = new SWF10_cl;

    var SWF11_domain = new Domain(Domain.currentDomain);
    SWF11_domain.load(test_path, 11);
    var SWF11_cl = SWF11_domain.getClass("testclass");
    var SWF11_helper = new SWF11_cl;

    var result;

    result = SWF10_helper.vtest(leaf_path, SWF10_domain, 10);
    Assert.expectEq("Test Vector.concat with SWF10 loading SWF10 behavior",
      "1,4,3,2",
      result.toString());

    result = SWF11_helper.vtest(leaf_path, SWF11_domain, 11);
    Assert.expectEq("Test Vector.concat with SWF11 loading SWF11 behavior",
      "1,2,3,4",
      result.toString());

    result = SWF10_helper.vtest(leaf_path, SWF10_domain, 11);
    Assert.expectEq("Test Vector.concat with SWF10 loading SWF11 behavior",
      "1,2,3,4",
      result.toString());

    result = SWF11_helper.vtest(leaf_path, SWF11_domain, 10);
    Assert.expectEq("Test Vector.concat with SWF11 loading SWF10 behavior",
      "1,4,3,2",
      result.toString());


}
