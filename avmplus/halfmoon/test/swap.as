// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
function swaptest(c,x,y) {
  while (c--) { var t = x; x = y; y = t; }
  print(x, y)
}
swaptest(2,0,1)
swaptest(1,0,1)
swaptest(0,0,1)
}
