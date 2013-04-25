/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// load asm
{
    use namespace "avmplus";
    use namespace "flash.utils";

    var fname = System.argv[0];
    if (fname==undefined) throw "no file name given";
    var fname = System.argv[0];
    var str = "var asm = " + File.read (fname+".asm");
    //print(str);
}

// eval ast

{
    use namespace "avmplus";
    use namespace Parse;
    use namespace Gen;
    //print ("parsing");
    var top = [];
    var parser = new Parser(str,top);
    var nd = parser.program();
    var bytes = cg(nd).getBytes();
    Util::loadBytes(bytes);
}

// decode it

{
    //print ("decoding");
    var abc = AbcDecode::abcFile (asm);  // defined by side effect
}

// cogen and write it

{
    var len = Util::writeBytesToFile(abc.getBytes(), fname+".abc");
    print (fname+", "/*+(t3-t1)+" ms, "*/+len+" bytes written");
}
