/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
 
import GetSetSameName.*;
import com.adobe.test.Assert;

  
//  var SECTION = "FunctionAccessors";
//  var VERSION = "AS3";
//  var TITLE   = "Function Accessors";
 var BUGNUMBER = "133108, 133828";
 


var g = new GetSetSameNameArg();
Assert.expectEq("Calling getter of same name as argument", 4, g.e);
Assert.expectEq("Calling setter of same name as argument", 5, (g.e = 5, g.e));


var h = new GetSetSame();


try {
    result = h.GetSetSameName.toString();
} catch (e1) {
    result = "exception";
    
}
// This will have to be changed when bug 133824 is fixed
Assert.expectEq("Calling getter of same name as package", "1,2,3", result);




try {
    h.GetSetSameName = [7,8,9];

    result = h.GetSetSameName.toString();
} catch (e2) {
    result = "exception";
    
}
// This will have to be changed when bug 133824 is fixed
Assert.expectEq("Calling setter of same name as package", "7,8,9", result);



try {
    ff = GetSetSameName.GetSetSameName.x.toString();

} catch (e3) {
    ff = "exception";
}

Assert.expectEq("Calling static getter in matching package/class", "1,2,3", GetSetSameName.GetSetSameName.x.toString());


try {
    GetSetSameName.GetSetSameName.x = [4,5,6];
    ff = GetSetSameName.GetSetSameName.x.toString();
} catch (e4) {
    ff = "exception";
}

Assert.expectEq("Calling static setter in matching package/class", "4,5,6", ff);

