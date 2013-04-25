#!/usr/bin/env python
# -*- Mode: Python; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab:

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys
import os
import string
import subprocess
import time

def usage():
    print 'Usage: testabc.py avmshell suiteDirectory'

def runAvm(shellExec, abcfile, options):
    shellExec += " -Dnodebugger -Dtimeout"
    for k in options:
        os.putenv(k, options[k])
    avmshellCommand = os.path.abspath(shellExec) + " " + abcfile
    avmshellProcess = os.popen(avmshellCommand)
    return avmshellProcess.read()

halfmoon_env = { 'MODE':'4', 'PROFILER':'1' }
baseline_env = { 'MODE':'0' }

def scrub(s):
    return s.strip(string.whitespace)

def checkResults(baselineResult, halfmoonResult, shortname):
    if (scrub(baselineResult) != scrub(halfmoonResult)):
        print "Halfmoon does not equal CodegenLIR for", shortname
        print "Shell:", baselineResult
        print "Halfmoon:", halfmoonResult
        return False
    return True

def runTest(abcfile, shortname):
    baselineResult = runAvm(shellExec, abcfile, baseline_env)
    halfmoonResult = runAvm(shellExec, abcfile, halfmoon_env)
    checkResults(baselineResult, halfmoonResult, abcfile)

def findTest(arg, dirname, files):
    print dirname
    for test in files:
        testLocation = os.path.abspath(dirname + "/" + test)
        if os.path.isfile(testLocation) and testLocation.endswith('.abc'):
            runTest(testLocation, test)

numberOfArgs = len(sys.argv)
if (numberOfArgs != 3):
    usage()
    exit(0)

shellExec = sys.argv[1]
testdir = sys.argv[2]

os.path.walk(testdir, findTest, "")
