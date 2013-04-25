// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function f1(k:int) {
  var i:int = 0
  var j:int = 0
  while (i < k) {
  	i = i + 1
	j = j + 1
  }
}
f1(1)

function f2(k:int) {
  var i:int = 0
  var j:int = 0
  while (i < k) {
  	i = i + 1
	j = j * 2
  }
}
f2(2)

function f3(k:int) {
  if (k)
    print(k)
  else
    print(k)
}
f3(3)

function f4(k:Number) {
  var x = k+k;
  var y = k+k;
  var z = k-x
  return z
}
f4(1)
