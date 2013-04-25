// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function verifyTest() {
  var result = 1.2718440192507248;
  var expectedResult = 1.2718440192507248;
  if (result !== expectedResult) {
    return false;
  } else {
    return true;
  }
}

if (verifyTest()) {
  print("passed");
}
