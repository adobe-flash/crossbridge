// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

function getTestCases() {
    var array1 = new Array();
    var array2 = [];
    var size   = 10;

    // this for loop populates array1 and array2 as follows:
    // array1 = [0,1,2,3,4,....,size - 2,size - 1]
    // array2 = [size - 1, size - 2,...,4,3,2,1,0]
    for (var i = 0; i < size; i++)
    {
        array1.push(i);
        array2.push(size - 1 - i);
    }

	print(String(array1) == String(array2));
    array1.sort();
    print(array2.sort());
	print(array1);
	print(array2);

	// GVN strips out both convert_s for array1 and array2
	print(String(array1) == String(array2));
}

getTestCases();
