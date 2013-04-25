// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

{

// extracted from esc/src/util.es.  Compile with
// esc:  avm esc.swf -- maketable.as

function makeTable(size) {
	var tbl = new Array(size);
	for ( var i=0; i < size; i = i + 1)
		tbl[i] = null;
	return tbl;
}
print(makeTable(3))

}
