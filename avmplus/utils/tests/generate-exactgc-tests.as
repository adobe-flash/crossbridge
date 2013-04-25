/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Generate some text files called exactgc-shouldpass-n.txt and
 * exactgc-shouldfail-n.txt.  These have various mixed line endings.
 * All the shouldpass files should be run through exactgc.abc without
 * failure.  All the shouldfail files should be run through
 * exactgc.abc with an error about mixed line endings.
 *
 * Invoke exactgc.abc eg like this:
 *
 *   avmshell exactgc.abc -- exactgc-shouldpass-1.txt
 *
 * Usually this does the trick:
 *
 *   avmshell generate-exactgc-tests.as
 *   for i in *should* ; do echo $i; avmshell ../exactgc.abc -- $i ; done  
 */

import avmplus.*;

/* More interesting cases are possible - consider eg a file that's
 * all-CR except the last line is terminated by LF - but at least
 * we're covering the obvious.
 */
var desc = [["LF", "\n"],
			["CR", "\r"],
			["CRLF", "\r\n"],
			["Mixed CR and LF", ["\r", "\n"]],
			["Mixed CR and CRLF", ["\r", "\r\n"]],
			["Mixed LF and CRLF", ["\n", "\r\n"]],
			["Mixed CR, LF and CRLF", ["\r", "\n", "\r\n"]]];

var ispass_cnt = 1;
var isfail_cnt = 1;
for ( var i=0 ; i < desc.length ; i++ ) {
	var v = desc[i];
	var sep = v[1];
	var ispass = !(sep is Array);
	var cnt = ispass ? ispass_cnt++ : isfail_cnt++;
	var fn = "exactgc-" + (ispass ? "shouldpass" : "shouldfail") + "-" + cnt + ".txt";

	if (!(sep is Array))
		sep = [sep];
	
	var s = "/* " + (ispass ? "Should pass: " : "Should fail: ") + v[0] + " */" + eol(sep);
	for ( var j=0 ; j < 100 ; j++ )
		s = s + "this is some text" + eol(sep);

	File.write(fn, s);
}

function eol(sep) {
	return sep[Math.floor(Math.random() * sep.length)];
}
