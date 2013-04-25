/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package Package1
{
    public const item1;
}

import Package1.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;


var thisError = "no error";
try
{
    item1 = "updated";
}
catch(err)
{
    thisError = err.toString();
}
finally
{
    Assert.expectEq("Attempt to modify package const globally", "ReferenceError: Error #1074", Utils.referenceError(thisError));
}

