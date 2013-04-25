/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil;  -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use namespace "flash.utils";
use namespace "avmplus";

// Placeholder class until we have decimal support

class decimal {
    const value;

    // s must be a valid floating point literal optionally prefixed
    // by + or -, no spaces on either side.
    function decimal(s) 
        : value=s 
    {
    }

    function toString()
        value;

    function asDouble()
        parseFloat(value);
}

Util function commandLineArguments() {
    return System.argv;
}
	
Util function writeStringToFile(s:String, filename:String) {
    let b = new ByteArray();
    b.writeUTFBytes(s);
    File.writeByteArray(filename, b);
}
    
Util function readStringFromFile(fn) {
    return File.read (fn);
}

Util function writeBytesToFile(bytes, filename) {
    File.writeByteArray(filename, bytes);
    return bytes.length;
}

Util function loadBytes(bytes) {
    return Domain.currentDomain.loadBytes(bytes);
}

