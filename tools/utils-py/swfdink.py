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

if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.set_defaults()
    optParser.add_option( '-c', '--compress', dest="compress", action="store_true", default=False)
    optParser.add_option( '-u', '--uncompress', dest="uncompress", action="store_true", default=False)
    optParser.add_option( '-v', '--version', dest="version", type="int", default='-1')
    (opts, args) = optParser.parse_args()

    if len(args) < 1:
        print "invalid usage"
        exit(1)
    infile = args[0]
    
    sf = open(infile)
    magic = sf.read(3)
    compressed = ( magic == "CWS")
    v = sf.read(1)
    ln = struct.unpack("I", sf.read(4))[0]
    data = sf.read()
    sf.close()
    
    out = open(infile, 'wb')
    
    if opts.compress:
        out.write("CWS")
    else:
        out.write("FWS")

    out.write(struct.pack("B", v if opts.version == -1 else opts.version))
    out.write(struct.pack("I", ln))

    if opts.compress and not compressed:
        data = zlib.compress(data)
    
    if not opts.compress and compressed:
        data = zlib.decompress(data)
        
    out.write(data)
