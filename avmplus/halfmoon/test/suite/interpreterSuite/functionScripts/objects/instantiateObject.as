// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function someObject(a, b, c) {
    this.a = a;
    this.b = b;
    this.c = c;
}

function createObject() {
    var x = new someObject(10, 20, 30);
    print(x.b);
}

createObject();
