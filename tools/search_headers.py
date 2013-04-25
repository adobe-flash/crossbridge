#!/usr/bin/python
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

import os
import subprocess
import re
import tempfile
from sys import argv, exit

includes= '-include "sys/cdefs.h" -include "sys/tty.h"'
asm_regex = re.compile('(\s(__)?asm(__)?\s)')
asm_exceptions = ['__IDSTRING', '_GLIBCXX_(READ|WRITE)_MEM_BARRIER',
                  '\.ident']
file_exceptions = ['AS3.h', 'AS3++.h', 'AS3++.impl.h', 'AS3Wig.h', 'AVM2.h', 'builtin.h', 'playerglobal.h']
asm_exceptions_regex = [re.compile(x) for x in asm_exceptions]
failedfiles = []
okfiles = []
badasmfiles = []
sdk = ''

def isexcepted(line):
    for r in asm_exceptions_regex:
        if re.search(r, line):
            return True
    return False


def hasbadasm(files):
    bad = False
    for f in files:
        file = open(f, 'r')
        contents = file.readlines()
        file.close()
        for line in contents:
            if re.search(asm_regex, line) and not isexcepted(line):
                print 'ASM: ' + line
                bad = True
    return bad


def handle(dir, relpath, basename):
    sdkcc = os.path.join(sdk, 'usr/bin/gcc')
    sdkcxx = os.path.join(sdk, 'usr/bin/g++')
    preprocessed = '{0}/{1}.i'.format(relpath, basename)
    macros = preprocessed + '.macros'
    cmds = [(' -E {2} -o {0} {1}', preprocessed), 
            (' -E {2} -dM -o {0} {1}', macros)]

    subprocess.call('mkdir -p ' + relpath, shell=True)
    for c in cmds:
        if relpath.find('c++') >= 0 or basename.find('++') >= 0:
            cmdline = sdkcxx + c[0]
        else:
            cmdline = sdkcc + c[0]

        headername = dir + '/' + basename
        cmdout = tempfile.TemporaryFile()
        if subprocess.call(cmdline.format(c[1], headername, includes), 
                           shell=True, stdout=cmdout, stderr=cmdout):
            cmdout.seek(0)
            errs = cmdout.read()

            if not basename in file_exceptions:
                if not '#error' in errs:
                    failedfiles.append(headername)
                    print 'preprocessing failed: ' + errs
                return

    if not basename in file_exceptions and hasbadasm([x[1] for x in cmds]):
        badasmfiles.append(headername)
    else:
        okfiles.append(headername)


if __name__ == "__main__":
    sdk = argv[1]
    includedir = os.path.join(sdk, 'usr/include')
    for root, dirs, files in os.walk(includedir):
        files = filter(lambda x: x.endswith('.h'), files)
        outpath = os.path.join(argv[2], os.path.relpath(root, includedir))
        for f in files:
            handle(root, outpath, f)

    code = 0
    if (len(failedfiles)):
        print 'Files that failed to preprocess:'
        print '\n'.join(failedfiles)
        code = 1
    if (len(badasmfiles)):
        print 'Files containing potentially bad assembly:'
        print '\n'.join(badasmfiles)
        code = 1

    exit(code)
