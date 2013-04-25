/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Package1
{
    public class Class1
    {
        private var classItem1 = "Class1 var classItem1 set at creation time";
    }
    
    public class Class2
    {
        public function getClass1ClassItem1()
        {
            var c1 = new Class1();
            return c1.classItem1;
        }
    }
}

import Package1.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;

var c2:Class2 = new Class2();


var thisError = "no error";
try
{
    c2.getClass1ClassItem1();
}
catch(err)
{
    thisError = err.toString();
}
finally
{
    Assert.expectEq("attempt to access private variable of Class1 in Class2", "ReferenceError: Error #1069", Utils.referenceError(thisError));
}
