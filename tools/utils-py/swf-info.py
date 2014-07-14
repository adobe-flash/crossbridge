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
import zlib

class zlibfile(object):
    def __init__(self, f, compressed=False):
        if compressed:
            d = zlib.decompressobj()
            self.data = d.decompress(f.read())
        else:
            self.data = f.read()
        f.close()
        self.pos = 0

    def read(self, num=1):
        result = self.data[self.pos:self.pos+num]
        self.pos += num
        return result

    def close(self):
        self.data = None

    def flush(self):
        pass

def readString(f):
    sdata = []
    b = f.read(1)
    while b[0] != '\0':
        sdata += b[0]
        b = f.read(1)
    return "".join(sdata)

def tagName(tag):
    if tag == 0:
        return "End"
    if tag == 1:
        return "ShowFrame"
    if tag == 65:
        return "ScriptLimits"
    if tag == 69:
        return "FileAttributes"
    if tag == 76:
        return "SymbolClass"
    if tag == 82:
        return "DoABC2"
    if tag == 87:
        return "DefineBinaryData"
    if tag == 92:
        return "Definesignature"

    return ""

if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.set_defaults()
    optParser.add_option( '-i', '--insertafter', dest="insertafter", default = 0, type="int")
    (opts, args) = optParser.parse_args()

    magic = open(args[0]).read(3)

    sf = open(args[0])
    sf.read(3)
    v = struct.unpack("b", sf.read(1))
    ln = struct.unpack("I", sf.read(4))

    if magic == "CWS":
        print "zlib compressed"
        f = zlibfile(sf, True)
    elif magic == "ZWS":
        print "lzma compressed"
        exit(1)
    elif magic == "FWS":
        print "uncompressed"
        f = zlibfile(sf, False)
    else:
        print "unknown magic bytes: %s" % magic
        exit(1)

    print "Version: %d" % v
    print "Length: %d" % ln

    rs = f.read(1)
    r = struct.unpack("B", rs)
    rbits = (r[0] & 0xff) >> 3
    rrbytes = (7 + (rbits*4) - 3) / 8;
    rect = f.read((int)(rrbytes))
    print "FrameSize: ..."

    fps = struct.unpack("H", f.read(2))
    print "FrameRate: %d" % fps
    fnum = struct.unpack("H", f.read(2))
    print "FrameCount: %d" % fnum
    
    tag = -1
    while tag != 0:
        rh = f.read(2)
        if rh is None or len(rh) is not 2:
            break
        recordheader = struct.unpack("BB", rh)
        code = ((recordheader[1]&0xff) << 8) | (recordheader[0]&0xff)
        tag = (code >> 6)
        length = code & 0x3f
        bhead = 2

        if length == 0x3f:
            ll = f.read(4)
            longlength = struct.unpack("BBBB", ll)
            length = ((longlength[3]&0xff) << 24) | ((longlength[2]&0xff) << 16) | ((longlength[1]&0xff) << 8) | (longlength[0]&0xff)
        
        print "Tag %d(%s) length %d" % (tag, tagName(tag), length)

        if tag == 65:
            maxRecursion = struct.unpack("H", f.read(2))
            print "- maxRecursion %d" % maxRecursion
            scriptTimeout = struct.unpack("H", f.read(2))
            print "- scriptTimeout %d" % scriptTimeout
        elif tag == 69:
            flags = struct.unpack("i", f.read(4))[0]
            print "- FileAttribute flag %d" % flags
        elif tag == 82:
            flags = struct.unpack("I", f.read(4))
            print "- flags %d" % flags
            abcname = readString(f)
            print "- name '%s'" % abcname
            f.read((length - 4) - (len(abcname) + 1))
        elif tag == 76:
            numsymbols = struct.unpack("H", f.read(2))[0]
            print "- numsymbols %d" % numsymbols
            while numsymbols > 0:
                tagid = struct.unpack("H", f.read(2))[0]
                tagname = readString(f)
                print "- %d : %s" % (tagid, tagname)
                numsymbols -= 1
        else:
            f.read(length)
