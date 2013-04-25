/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

print("memstats startup PASSED!");
var array:Array=[]
for (var i=0;i<10;i++)
   array.push(new Object());

print("memstats after allocations PASSED!");
