// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

var takResult = new Array();

for ( var i = 3; i <= 5; i++ ) {
    takResult['tak' + i] = i;
}

print(takResult['tak' + 4]);

