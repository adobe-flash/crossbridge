/* -*- mode: java -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Utility to stitch ABC files together into a single swf.
 *
 * Usage:
 *
 *   swfmake [-c] [-o filename] abcfile ...
 *
 *   -o filename    defines the name of the output file
 *
 *   -c             creates a compressed swf
 *
 * By default the name of the output file is the name of the first abc
 * file with ".abc" replaced by ".swf", and the swf file is uncompressed.
 */

package swfmake
{
    import avmplus.*;
    import flash.utils.*;

    var input_names = [];
    var output_name = null;
    var compress = undefined;

    var i=0, j;
    var argc=System.argv.length;
    var argv=System.argv;
    var s;
    while (i < argc) {
	s = argv[i];
	if (s == "-o") {
	    if (output_name != null || i+1 == argc)
		usage();
	    output_name = argv[i+1];
	    i += 2;
	}
	else if (s == "-c") {
	    if (compress is Boolean)
		usage();
	    compress = true;
	    i++;
	}
	else if (s.charAt(0) == "-") {
	    usage();
	}
	else {
	    input_names.push(s);
	    i++;
	}
    }

    if (!(compress is Boolean))
	compress = false;

    if (input_names.length == 0)
	usage();

    if (output_name == null) {
	s = input_names[0];
	if (s.match(/\.abc$/))
	    output_name = s.replace(/\.abc$/, ".swf");
	else
	    output_name = s + ".swf";
    }

    var body = new ByteArray();
    body.endian = "littleEndian";
    body.writeByte(2 << 3);   // RECT: 2 bits per dim
    body.writeByte(0);        //   and they're all zero
    body.writeShort(0);       // framerate
    body.writeShort(0);       // framecount

    for ( i=0 ; i < input_names.length ; i++ ) {
	s = input_names[i];
	var bytes = File.readByteArray(s);
	body.writeShort((82 << 6) | 63);  // DoABC, extended length
	body.writeUnsignedInt(bytes.length + 4 + s.length + 1);
	body.writeUnsignedInt(0);         // flags
	for ( j=0 ; j < s.length ; j++ )
	    body.writeByte(s.charCodeAt(j) & 255);
	body.writeByte(0);
	body.writeBytes(bytes);
    }
    body.writeShort(0);                   // End
    
    var numbytes = body.length + 8;

    if (compress)
	body.compress();

    var result = new ByteArray();
    result.endian = "littleEndian";
    result.writeByte((compress ? 'C' : 'F').charCodeAt(0));
    result.writeByte('W'.charCodeAt(0));
    result.writeByte('S'.charCodeAt(0));
    result.writeByte(9);
    result.writeUnsignedInt(numbytes);
    result.writeBytes(body);
    File.writeByteArray(output_name, result);
    System.exit(0);

    function usage() {
	print("Usage: avmshell swfmake.abc -- [-c] [-o filename] abcfile ...");
	System.exit(1);
    }
}