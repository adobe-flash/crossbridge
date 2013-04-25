/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use namespace "avmplus";
use namespace AbcEncode;

// load ast
{
    use namespace "flash.utils";

    var fname = System.argv[0];
    var verbose = false;
    if( fname == "verbose" )
    {
        fname = System.argv[1];
        verbose = true;
    }
    if (fname==undefined) throw "no file name given";
    var bytes = ByteArray.readFile(fname+".abc");
}

{
    bytes = Abc::parseAbcFile(new ABCByteStream(bytes));

    print ("encoding asm");
    var tx = AbcEncode::abcFile (bytes, verbose);
    Util::writeStringToFile (tx,fname+".asm");
    print (fname+".asm, "+tx.length+" chars written");
}
