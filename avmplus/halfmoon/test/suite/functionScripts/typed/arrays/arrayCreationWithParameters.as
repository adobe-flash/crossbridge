// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function core_md5():Array
{
  var a:int =  1732584193;
  var b:int = -271733879;

  var resultArray:Array = Array(a, b);
  print("In core md5: " + resultArray[0]);
    return resultArray;
}

core_md5();
