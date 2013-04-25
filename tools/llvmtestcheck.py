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
import filecmp

def runProcess(p, msg, ignoreErrors = False):
    (stdoutdata, stderrdata) = p.communicate()
    if not ignoreErrors:
        if not p.returncode == 0:
            if stderrdata:
                print stderrdata
            print msg
            sys.exit(1)
    return (stdoutdata, stderrdata, p.returncode)

def createProcess(exe, args, verbose = False):
    cmdargs = [exe] + args
    
    if verbose:
        print "# running: " + " ".join(cmdargs)
    
    return subprocess.Popen(cmdargs, executable=exe, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def hashOutput(outputfile):
    makefile = os.path.join(
        os.path.dirname(os.path.dirname(outputfile)),
        'Makefile')
    if os.path.exists(makefile):
        with open(makefile) as mk:
            mklines = mk.readlines()
            for line in mklines:
                if (line.find('HASH_PROGRAM_OUTPUT = 1') >= 0 or
                        line.find('HASH_PROGRAM_OUTPUT := 1') >= 0):
                    return True
    return False

def outputEquals(a, b, cmpExe, cmpArgs):
    tmpA = tempfile.NamedTemporaryFile()
    tmpA.write("".join(a))
    tmpA.flush()
    tmpB = tempfile.NamedTemporaryFile()
    tmpB.write("".join(b))
    tmpB.flush()
    (stdout, stderr, retcode) = runProcess(createProcess(cmpExe, cmpArgs + [tmpA.name, tmpB.name], False), None, True)
    return retcode == 0

if __name__ == "__main__":
    optParser = optparse.OptionParser()
    optParser.set_defaults()
    optParser.add_option( '--srcdir', dest="src", default = None)
    optParser.add_option( '--builddir', dest="build", default = None)
    optParser.add_option( '--rerun', dest="rerun", default = False, action = "store_true")
    optParser.add_option( '--fpcmp', dest="fpcmp", default = None)
    (opts, args) = optParser.parse_args()
    if opts.fpcmp != None:
      cmpExe = opts.fpcmp
    else:
      cmpExe = opts.build + '/../../../llvm-debug/bin/fpcmp'
    cmpArgs = ['-r', '0.01'] # 1% variance ok!
    hashScript = opts.src + '/HashProgramOutput.sh'

    for (p, n, fs) in os.walk(opts.build):
        for f in fs:
            if f.endswith('.llc'):
                exe = p + '/' + f
                if not "Output" in exe:
                    continue
                refout =  opts.src + '/' +p[len(opts.build):-7] + '/' + f[:-4] + '.reference_output'

                outputfile = None

                if opts.rerun:
                    (stdout, stderr, retcode) = runProcess(createProcess(exe, []), None, True)
                else:
                    outputfile = exe[:-4] + '.out-llc'
                    if os.path.exists(outputfile):
                        if hashOutput(outputfile):
                            hashed = outputfile + '.hash'
                            shutil.copy(outputfile, hashed)
                            runProcess(createProcess(hashScript, [hashed]), None)
                            outputfile = hashed
                        if os.path.getsize(outputfile) > 500000000:
                            stdout = "Actual output too big, see %s" % outputfile
                        else:
                            stdout = open(outputfile).readlines()
                        stderr = ""
                        if stdout[-1].startswith('RunSafely.sh'):
                            stdout = stdout[:-1]
                        stdout = "".join(stdout)
                    else:
                        stdout = ""

                if os.path.exists(refout):
                    print "=========="
                    
                    if os.path.exists(refout + "." + sys.platform):
                        print "# using platform specific reference output: %s" % sys.platform
                        refout = refout + "." + sys.platform

                    actual = stdout.splitlines(True)
                    expected = open(refout).readlines()

                    i = 0
                    for line in actual:
                        actual[ i ] = line.strip() + "\r\n"
                        i = i + 1
                    i = 0
                    for line in expected:
                        expected[ i ] = line.strip() + "\r\n"
                        i = i + 1
                    testPassed = len(actual) == len(expected) and outputEquals(actual, expected, cmpExe, cmpArgs)
                    
                    if not testPassed and outputfile is not None:
                        testPassed = filecmp.cmp(outputfile, refout)
                        if testPassed:
                            print "-- Binary comparison succeeded"

                    if testPassed:
                        print "PASS: %s" % exe
                    else:
                        print "FAIL: %s" % exe
                        print "reference output: (%s)" % refout
                        for line in expected:
                            print line.strip()
                        print "actual output: "
                        for line in actual:
                            print line.strip()
                    
