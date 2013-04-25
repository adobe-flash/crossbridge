// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function bigSwitch(switchCase) {
    switch (switchCase) {
    case 3:
        print("three");
        break;
    case "hello":
        print("world");
        break;
    default:
        print("default here");
        break;
    }
}

bigSwitch(5);
