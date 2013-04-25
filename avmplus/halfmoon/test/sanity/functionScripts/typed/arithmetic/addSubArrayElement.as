// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
  var loops:int = 15
  var nx:int = 120
  var nz:int = 120

  function run3dMorph():int {    
    var a:Array = new Array(nx*nz*3);
    a[0] = 0;
    var testOutput:Number = 0;

    for (var i:int = 0; i < 1; i++)
        testOutput += a[i];

    testOutput = testOutput - 6.7;
  }

run3dMorph();

}
