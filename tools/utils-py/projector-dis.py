#!/usr/bin/env python
#
# Copyright (c) 2013 Adobe Systems Inc

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


from __future__ import division

import os
import sys
import tempfile
import optparse
import subprocess
import shutil
import struct

if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.set_defaults()
    optParser.add_option( '-o', '--output', dest="out", default = "output")
    (opts, args) = optParser.parse_args()

    if len(args) != 1:
        print "usage: projector-dis [--output foo] avmprojector"
        exit(1)

    hlen = 12
    data = open(args[0]).read()

    (m1,m2,m3,m4,clen, alen) = struct.unpack("BBBBII", data[-hlen:])

    if not (m1 == 0x56 and m2 == 0x34 and m3 == 0x12 and m4 == 0xFA):
        print "That file is not a valid AVM projector."
        exit(2)

    print "content length: %d" % clen
    if alen == 2147483647:
        print "vm args not specified"
        alen = 0
    else:
        astr = data[-alen - hlen:-hlen]
        print "vm args (%d): %s" % (alen, astr)
    
    print "saving avmshell to %s" % (opts.out + ".avmshell")
    f = open(opts.out + ".avmshell", 'wb')
    f.write(data[:-alen-clen-hlen])
    f.close()
    

    content = data[-clen-alen-hlen:-alen-hlen]
    if content[:3] == "FWS":
        cfile = (opts.out + ".swf")
        ctype = "SWF"
    elif content[:3] == "CWS":
        cfile = (opts.out + ".swf")
        ctype = "SWF(compressed)"
    else:
        cfile = (opts.out + ".abc")
        ctype = "ABC"
    
    print "saving %s to %s" % (ctype, cfile)
    f = open(cfile, 'wb')
    f.write(content)
    f.close()

    if ctype == "SWF":
        f = open(cfile)
        f.read(8)
        r = struct.unpack("B", f.read(1))
        rbits = (r[0] & 0xff) >> 3
        rrbytes = (7 + (rbits*4) - 3) / 8;
        f.read((int)(rrbytes) + 4)

        tag = -1
        abcid = 0
        while tag != 0:
            recordheader = struct.unpack("BB", f.read(2))
            code = ((recordheader[1]&0xff) << 8) | (recordheader[0]&0xff)
            tag = (code >> 6)
            length = code & 0x3f
            bhead = 2
            longlength = None

            if length == 0x3f:
                longlength = struct.unpack("BBBB", f.read(4))
                length = ((longlength[3]&0xff) << 24) | ((longlength[2]&0xff) << 16) | ((longlength[1]&0xff) << 8) | (longlength[0]&0xff)
                bhead += 4

            if tag == 82 or tag == 72: # ABC Data
                data = f.read(length)
                n = 0
                
                if tag == 82: # skip over string data
                    n = 4
                    while data[n] != '\0':
                        n += 1
                    n += 1
                abc = data[n:]
                abcfile = "%s.%04d.abc" % (cfile, abcid)
                print "saving ABC to %s" % abcfile
                o = open(abcfile, 'wb')
                o.write(abc)
                o.close()
                abcid += 1
            else:
                f.read(length)
