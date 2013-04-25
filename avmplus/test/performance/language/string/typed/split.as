/*
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

include '../timetest.as'

if (CONFIG::desktop) {
    var size:int = 50000;
} else {
    var size:int = 40000;
}

var aString:String = "a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890";

for (var k:int = 0; k < size; k++)
    aString += "a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu vwxyz12 34567890 a bc def ghij klmno pqrstu "


function split(loops:int):void {
    aString.split(" ");
}

timetest(split);