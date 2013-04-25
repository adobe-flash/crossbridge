#!/usr/bin/env python

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys
import os
import string
import subprocess
import time
import glob
import re

iterations = 1
resultsFile = "benchmarkResults.csv"

nanojitResults = {}
halfmoonResults= {}

def usage():
    print("Usage")
    print("timeTestSuite.py avmshell asc.jar builtinAbc testFile")
    print

def runSomething(shellExec, testFile, vmArgs):
    total = 0
    avmshellCommand = os.path.abspath(shellExec) 
    callArgs = [avmshellCommand, "-Dnodebugger", testFile]
    for i in range(0, iterations):
        start = time.time() 
        avmshellProcess = subprocess.call(callArgs)
        end = time.time()
        total += end - start
        
    average = total / iterations
    return average

def runShell(shellExec, testFile, vmArgs):
    os.putenv("MODE", "0")
    return runSomething(shellExec, testFile, vmArgs)

def runHalfmoon(shellExec, testFile, vmArgs):
    os.putenv("MODE", "4")
    return runSomething(shellExec, testFile, vmArgs)

def compileAbc(ascJarFile, builtinAbc, testFile):
    ascJarFile = os.path.abspath(ascJarFile)
    builtinAbc = os.path.abspath(builtinAbc)
    ascCommand = "java -jar " + ascJarFile + " -import " + builtinAbc + " " + testFile
    compileToAbcProcess = os.popen(ascCommand)
    compiledFileName = testFile.replace(".as", ".abc")
    return compiledFileName

def writeResultsToFile(testFile):
    print("looking at file: " + testFile)
    avmshellResult = nanojitResults[testFile]
    halfmoonResult = halfmoonResults[testFile]
    halfmoonComparison = str(avmshellResult / halfmoonResult) 
    if (re.search('\\\\untyped\\\\', testFile)):
        typedFileName = re.sub('\\\\untyped\\\\', '\\\\typed\\\\', testFile)

    csvFile.write(testFile + "," + str(avmshellResult) + "," + str(halfmoonResult) + "," + str(halfmoonComparison) + "\n")


def runTest(shellExec, ascJarFile, builtinAbc, testFile):
    print("Running test: " + testFile);
    compiledFileName = compileAbc(ascJarFile, builtinAbc, testFile)
    # Note, last arg is vmargs eg -Dverbose=. atm unsupported
    shellResult = runShell(shellExec, compiledFileName, "")
    halfmoonResult = runHalfmoon(shellExec, compiledFileName, "")

    nanojitResults[testFile] = shellResult
    halfmoonResults[testFile] = halfmoonResult

def writeFileHeader(openedFile):
    openedFile.write("Benchmark, NanoJIT, Halfmoon, Difference\n")

numberOfArgs = len(sys.argv)
if (numberOfArgs != 5):
    usage()
    exit(0)

csvFile = None 
if (os.path.exists(resultsFile)):
    csvFile = open(resultsFile, "a")
else:
    csvFile = open(resultsFile, "w")
    writeFileHeader(csvFile)

def runSuite(shellExec, ascJarFile, builtinAbc, testDirectory):
    for file in os.listdir(testDirectory):
        fullPath = os.path.normpath(os.path.join(testDirectory, file))
        if (os.path.isdir(fullPath)):
            runSuite(shellExec, ascJarFile, builtinAbc, fullPath)
        elif (re.search('\.as', fullPath)):
            runTest(shellExec, ascJarFile, builtinAbc, fullPath)


def writeResults(testDirectory):
    for file in os.listdir(testDirectory):
        fullPath = os.path.normpath(os.path.join(testDirectory, file))
        if (os.path.isdir(fullPath)):
            writeResults(fullPath)
        # don't want .abc files
        elif (re.search('\.as$', fullPath)):
            writeResultsToFile(fullPath)

runSuite(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
writeResults(sys.argv[4])
csvFile.close()
