// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function fib(someNumber) {
    if (someNumber == 1) {
        return 1;
    } else if (someNumber == 0) {
        return 0;
    } else {
        return fib(someNumber - 1) + fib(someNumber -2);
    }
}

print(fib(8));
