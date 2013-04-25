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
import genperm

# 
# Download from http://www.parallelpython.com/downloads/pp/pp-1.6.1.zip
#
# Then run:
#
# python setup.py build
# python setup.py install
#
# Add the pp-1.6.1 directory to your PYTHONPATH environment variable

import pp

def usage():
    print 'Usage: testabc.py avmshell suiteDirectory numCPUs'

def runAvm(shellExec, abcfile, options):
    shellExec += " -Dnodebugger"
    for k in options:
        os.putenv(k, options[k])
    avmshellCommand = os.path.abspath(shellExec) + " " + abcfile
    avmshellProcess = os.popen(avmshellCommand)
    return avmshellProcess.read()


halfmoon_env = { 'MODE':'4' }
halfmoon_interp_env = { 'MODE':'2' }
baseline_env = { 'MODE':'0' }

jobs = []

def scrub(s):
    return s.strip(string.whitespace)

def checkResults(baselineResult, baseline_env, halfmoonResult, halfmoon_env, shortname):
    if (scrub(baselineResult) != scrub(halfmoonResult)):
        print  "Halfmoon does not equal baseline for" + shortname
        print "Shell:", baselineResult
        print "Shell options: ", baseline_env
        print "Halfmoon:", halfmoonResult
        print "Halfmoon options: ", halfmoon_env
        return False
    return True

def runTest(shellExec, baseline_env, halfmoon_env, abcfile, shortname):
    baselineResult = runAvm(shellExec, abcfile, baseline_env)
    halfmoonResult = runAvm(shellExec, abcfile, halfmoon_env)
    checkResults(baselineResult, baseline_env, halfmoonResult, halfmoon_env, abcfile)

def findTest(job_server, dirname, files):
    print dirname
    for test in files:
        testLocation = dirname + "/" + test
        if os.path.isfile(testLocation) and testLocation.endswith('.abc'):
            for run in runs:
                halfmoon_vars = '{'
                for env_var in run[0]:
                    name, val = env_var.split('=')
                    halfmoon_vars += "'"
                    halfmoon_vars += name
                    halfmoon_vars += "' : '"
                    halfmoon_vars += val
                    halfmoon_vars += "'"
                    halfmoon_vars += ","
                halfmoon_vars += '}'
                halfmoon_env = eval(halfmoon_vars)
                jobs.append(job_server.submit(runTest, (shellExec, baseline_env, halfmoon_env, testLocation, test), (runAvm,), globals=globals()))

numberOfArgs = len(sys.argv)
if (numberOfArgs < 3):
    usage()
    exit(0)

shellExec = sys.argv[1]
testdir = sys.argv[2]
print "numberOfArgs: " + str(numberOfArgs)
print "sys.argv[1]: " + sys.argv[1] 
print "sys.argv[2]: " + sys.argv[2] 
if (numberOfArgs >= 4):
    print "sys.argv[3]: " + sys.argv[3] 
    ncpus = int(sys.argv[3])
else:
    ncpus = 2
    
shellExec += ' -Dnodebugger '

runs = genperm.genperm('avmdiff2.cfg')

job_server = pp.Server()
job_server.set_ncpus(ncpus)
print "Starting ", job_server.get_ncpus(), " workers"

os.path.walk(testdir, findTest, job_server)

job_server.wait()
for job in jobs:
    job()
    
job_server.print_stats()
