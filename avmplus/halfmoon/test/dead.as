// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// dead loop - can we remove it?

function dead1(c) {
  if (c) {} else {}
}
dead1(0)

function dead2(c) {
  if (c) { print(c) }
}
dead2(1)

function dead3(c:int) {
  var t1 = getTimer()
  while (--c >= 0) {}
  print(getTimer() - t1)
}
dead3(10000000)

function dead4(c:int):int {
  var x:int
  if (c > 0) {
    x = 1; while (--c >= 0) {}
  } else {
    x = 0
  }
  return x
}
dead4(2)
