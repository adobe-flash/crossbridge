#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

import os, sys, getopt, datetime, pipes, glob, itertools,socket
import tempfile, string, re, platform, traceback
import subprocess, math, types
from os.path import *
from os import getcwd,environ,walk
from datetime import datetime
from glob import glob
from sys import argv, exit
from getopt import getopt
from itertools import count
from time import time

# add parent dir to python module search path
sys.path.append('..')

from util.convertAcceptanceToJunit import *
try:
    from util.runtestBase import RuntestBase
    from util.runtestUtils import *
except ImportError:
    print("Import error.  Please make sure that the test/acceptance/util directory has been deleted.")
    print("   (directory has been moved to test/util).")

# Constants
DEFAULT_TRUNCATE_LEN = 6  # default len of number fields when displaying results

class PerformanceRuntest(RuntestBase):
    avm2 = ''
    avmname = ''
    avmDefaultName = 'avm'
    avm2DefaultName = 'avm2'
    besttime = 'best'
    besttime2 = 'best2'
    avm2name = ''
    currentDir = ''
    displayMetrics = []
    iterations = 1
    vmname = 'unknown'  # name sent to socketserver
    memory = False
    avmrevision = ''
    avm2version = ''
    avm2revision = ''
    detail = False
    fullpath = False
    raw = False
    repo = ''
    vmargs2 = ''
    optimize = True
    perfm = False
    tmpfile = None
    osName = ''
    logresults = False
    logConfigAppend = ''
    socketlogFile = None
    socketlogFailure = False
    serverHost = '10.60.48.47'
    serverPort = 1188
    finalexitcode = 0
    
    # testData structure:
    # { testName : { metric : { results1/2 : [], best1/2 : num, avg1/2 : num, spdup : num }}}
    testData = {}   # dict that stores all test results and calculations
    
    csvAppend = False
    metricInfo = {} # dict that holds info on each metric run
    currentMetric = ''

    indexFile = ''      # indexFile that is used to compute normalized results
    testIndexDict = {}  # dict used to store test indexes
    saveIndex = False   # save results from avm1 to saveIndexFile
    score = False       # compute scores ?
    
    # score1 & 2  hold the geometric mean of all tests run.  Means are kept seperate for each
    # metric, so the dictionary keeps a key for each metric:
    # e.g: {'metric' : {'score':runningScore, 'count':count}, 'v8' : {'score':3.14, 'count':1} }
    score1 = {}
    score2 = {}

    # formatting vars
    testFieldLen = 27   # field length for test name and path
    resultWidth = 8     # width of result columns

    # dict for storing aot compile times
    aot_compile_times={}

    # Index file header
    testIndexHeaderString = '''
# The testindex file contains a list of test results that are used to normalize
# all current results using these provided values.  This enables us to equally
# weight the results of each test irrespective of the actual test runtime.

# In order to simplify the importing and processing of this file, the values are
# kept in a python dictionary which MUST be named testIndexDict

# The dictionary format is:
# 'testName':{'metric':value}
'''

    def __init__(self):
        RuntestBase.__init__(self)
    
    def __str__(self):
        return 'PerformanceRuntest'

    def run(self):
        self.setEnvironVars()
        self.loadPropertiesFile()
        self.setOptions()
        self.parseOptions()
        self.altsearchpath='../../other-licenses/test/performance/'
        self.setTimestamp()
        self.checkPath(['avm2'])
        self.determineOS()
        # Load the root testconfig file
        self.settings, self.directives = self.parseTestConfig(self.testconfig)
        self.tests = self.getTestsList(self.args)
        if self.rebuildtests:
            self.rebuildTests()
            exit()
        # Load root .asc_args and .java_args files
        self.parseRootConfigFiles()
        self.loadMetricInfo()
        self.preProcessTests()
        self.printHeader()
        self.runTests(self.tests)
        if self.csv:
            self.outputCsvToFile()
        if self.score:
            self.printScoreSummary()
        if self.saveIndex:
            self.outputTestIndexFile()
        if self.junitlog:
            outfile=convertPerformanceToJunit(self.junitlog+'.txt',self.junitlog+'.xml',self.junitlogname)
            print("wrote results in junit format to %s" % outfile)
        #self.cleanup()

    def getTestsList(self, args):
        '''If an index file is being used, we only run the files in the index list'''
        # when --saveIndex is specified along with --index, run the specified files
        # and merge it with the --index values
        if self.indexFile and not self.saveIndex:
            indexTests = sorted(self.testIndexDict.keys())
            if args[0] == '.':
                return indexTests
            else:
                # only run the union of tests in indexTests and testsToRun
                testsToRun = list(set(indexTests) &
                                  set(RuntestBase.getTestsList(self, args)))
                if not testsToRun:
                    exit('There are no tests in the indexfile %s that match %s'
                         % (self.indexFile, args))
                return testsToRun
                
        return RuntestBase.getTestsList(self, args)


    def setEnvironVars(self):
        RuntestBase.setEnvironVars(self)
        if 'AVM2' in environ:
            self.avm2 = environ['AVM2'].strip()
        if 'VMARGS2' in environ:
            self.vmargs2 = environ['VMARGS2'].strip()


    def usage(self, c):
        RuntestBase.usage(self, c)
        print(" -S --avm2          second avmplus command to use")
        print("    --avmname       nickname for avm to use as column header")
        print("    --avm2name      nickname for avm2 to use as column header")
        print("    --detail        display results in 'old-style' format")
        print("    --raw           output all raw test values")
        print(" -i --iterations    number of times to repeat test")
        print(" -l --log           logs results to a file")
        print(" -k --socketlog     logs results to a socket server")
        print(" -r --runtime       name of the runtime VM used, including switch info eg. TTVMi (tamarin-tracing interp)")
        print(" -m --memory        logs the high water memory mark")
        print("    --metrics=      display specified metrics: either a comma-separated list of")
        print("                    metrics names (e.g. v8), or the keyword all.")
        print("    --vmversion     specify vmversion e.g. 502, use this if cannot be calculated from executable")
        print("    --vm2version    specify version of avm2")
        print("    --vmargs2       args to pass to avm2, if not specified --vmargs will be used")
        print("    --nooptimize    do not optimize files when compiling")
        print("    --perfm         parse the perfm results from avm")
        print("    --csv=          also output to csv file, filename required")
        print("    --csvappend     append to csv file instead of overwriting")
        print("    --score         compute and print geometric mean of scores")
        print("    --index=        index file to use (must end with .py)")
        print("    --saveindex=    save results to given index file name")
        print("    --fullpath      print out full path for each test")
        print("    --repo=         repository url (used when logging to performance db)")
        print("    --logConfigAppend= string to append to the config string that is logged to the database along with vmargs")
        exit(c)

    def setOptions(self):
        RuntestBase.setOptions(self)
        self.options += 'S:i:lkr:mp'
        self.longOptions.extend(['avm2=','avmname=','avm2name=','iterations=','log=','socketlog',
                                 'runtime=','memory','metrics=','larger','vmversion=', 'vm2version=',
                                 'vmargs2=','nooptimize', 'score', 'saveindex=', 'index=',
                                 'perfm','csv=', 'csvappend','prettyprint', 'detail', 'raw',
                                 'fullpath', 'repo=', 'logConfigAppend='])

    def parseOptions(self):
        opts = RuntestBase.parseOptions(self)
        for o, v in opts:
            if o in ('-S', '--avm2'):
                self.avm2 = v
            elif o in ('--avmname',):
                self.avmname = v
            elif o in ('--avm2name',):
                self.avm2name = v
            elif o in ('-i', '--iterations'):
                try:
                    self.iterations = int(v)
                except ValueError:
                    print('Incorrect iterations value: %s\n' % v)
                    self.usage(2)
            elif o in ('-l','--log'):
                self.logFileType='log'
                self.createOutputFile()
            elif o in ('-k', '--socketlog'):
                self.logresults = True
            elif o in ('-r', '--runtime'):
                self.vmname = v
            elif o in ('-m', '--memory'):
                self.memory = True
            elif o in ('--metrics'):
                self.displayMetrics = v.strip().lower().split(',')
                if 'memory' in self.displayMetrics:
                    self.memory = True
                    del self.displayMetrics[self.displayMetrics.index('memory')]
            elif o in ('--vmversion',):
                self.avmrevision = self.avmversion = v
            elif o in ('--vm2version',):
                self.avm2revision = self.avm2version = v
            elif o in ('--vmargs2',):
                self.vmargs2 = v
            elif o in ('--nooptimize',):
                self.optimize = False
            elif o in ('--perfm',):
                self.perfm = True
                self.resultWidth = 8    # perfm results can be pretty large
            elif o in ('--csv',):
                self.csv = True
                self.csvfile = v
            elif o in ('--csvappend',):
                self.csvAppend = True
            elif o in ('--score',):
                self.score = True
            elif o in ('--detail',):
                self.detail = True
            elif o in ('--raw',):
                self.raw = True
            elif o in ('--index',):
                self.loadIndexFile(v)
            elif o in ('--saveindex',):
                self.saveIndex = True
                self.saveIndexFile = v
            elif o in ('--fullpath',):
                self.fullpath = True
            elif o in ('--repo',):
                self.repo = v
            elif o in ('--logConfigAppend',):
                self.logConfigAppend = v

        self.avmname = self.avmname or self.avmDefaultName
        self.avm2name = self.avm2name or self.avm2DefaultName

    def loadIndexFile(self, indexFile):
        # The indexFile contains values used to normalize
        # the test run results.  For simplicity it is a python
        # file that defines a single dictionary: testIndexDict
        # That dicitonary is dynamically loaded here and then assigned
        # to the classvar of the same name
        self.indexFile = indexFile[:-3] if indexFile.endswith('.py') else indexFile
        try:
            exec('from %s import testIndexDict' % self.indexFile)
            self.testIndexDict = testIndexDict
        except ImportError:
            # TODO: friendlyfy this error message
            print('Error attempting to import %s:' % self.indexFile)
            raise

    def compile_test(self, as_file):
        if not isfile(self.shellabc):
            exit("ERROR: shell.abc %s does not exist, SHELLLABC environment variable or --shellabc must be set to shell.abc" % self.shellabc)
        args = []
        args.append('-import %s' % self.shellabc)
        if self.optimize:
            args.append('-optimize -AS3')
        debugoutput = []
        RuntestBase.compile_test(self, as_file, args, debugoutput)
        self.printOutput(None, debugoutput)
        if self.aotsdk and self.aotout:
            if isfile(splitext(as_file)[0] + ".abc"):
                startTime=time()
                RuntestBase.compile_aot(self, splitext(as_file)[0] + ".abc")
                self.aot_compile_times[as_file]=time()-startTime
   
    def socketlog(self, msg):
        if not self.socketlogFile:
            file="socketlog-%s.txt" % self.avmversion
            ctr=0
            while os.path.exists(file):
                ctr += 1
                file = "socketlog-%s-%s.txt" % (self.avmversion,ctr)
            self.socketlogFile=file
        open(self.socketlogFile,'a').write(msg)
        if self.socketlogFailure == False:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    # create a TCP socket
                s.settimeout(10) # set socket timeout to 10s
                s.connect((self.serverHost, self.serverPort)) # connect to server on the port
                s.send("%s;exit\r\n" % msg)         # send the data
                data = s.recv(1024)
                #print('Sent: %s' % msg)
                #print('Received: %s \n\n' % data)
                #s.shutdown(SHUT_RDWR)
                s.close()
            except :
                print("ERROR: Socket error occured:")
                print(sys.exc_info())
                print('buildbot_status: WARNINGS')
                self.socketlogFailure = True
                self.finalexitcode = 1

    def loadMetricInfo(self):
        '''load metric information from metricinfo.py'''
        try:
            from metricinfo import metric_info
        except ImportError:
            print('Error loading metricinfo.py file.')
            raise

        # verify that the loaded metric_info dictionary is valid
        for metric in metric_info:
            if 'best' not in metric_info[metric]:
                print('\nWarning: metricinfo.py - the %s metric does not have a "best" key defined - defaulting to min\n' % metric)
            elif not isinstance(metric_info[metric]['best'],(types.FunctionType,types.BuiltinFunctionType)):
                print('\nWarning: metricinfo.py - the "best" value for the %s metric must be a function - defaulting to min' % metric)
            # if the metric does not have a largerIsFaster value defined, default it to False
            if 'largerIsFaster' not in metric_info[metric]:
                metric_info[metric]['largerIsFaster'] = False
        
        self.metricInfo = metric_info

    def preProcessTests(self):
        'Code that must be executed before beginning a testrun'

        if self.logresults:
            # determine current config string for socketlog
            tmpvmargs = self.vmargs.replace("-AOTSIZE", "") # This is a fake vmarg that is used for reporting filesize, do not log to db
            self.log_config = "%s" % tmpvmargs.replace(" ", "")
            self.log_config = "%s" % self.log_config.replace("\"", "")
            if self.log_config.find("-memlimit")>-1:
                self.log_config=self.log_config[0:self.log_config.find("-memlimit")]
            self.log_config = self.log_config.replace("-memstats","")
            self.log_config += self.logConfigAppend
            
        if not self.aotsdk:
            self.checkExecutable(self.avm, 'AVM environment variable or --avm must be set to avmplus')
            if not self.avmversion:
                self.avmversion = self.getAvmVersion(self.avm)
            if not self.avmrevision:
                self.avmrevision = self.getAvmRevision(self.avmversion)
            if self.avm2:
                self.checkExecutable(self.avm2, '--avm2 must be set to avmplus')
                if not self.avm2version:
                    self.avm2version = self.getAvmVersion(self.avm2)
                if not self.avm2revision:
                    self.avm2revision = self.getAvmRevision(self.avm2version)
            else:   # only one avm being run
                self.testFieldLen = 50


    def printHeader(self):
        'Print run info and headers'
        self.js_print('Executing %d test(s)' % len(self.tests), overrideQuiet=True)
        self.js_print("%s: %s %s version: %s" % (self.avmname, self.avm, self.vmargs, self.avmversion))
        if self.avm2:
            self.js_print("%s: %s %s version: %s" % (self.avm2name, self.avm2, self.vmargs2, self.avm2version))
        self.avmDefaultName += ':'+self.avmrevision
        self.js_print('iterations: %s' % self.iterations)
        if self.indexFile:
            self.js_print('index mode enabled, values of -1 indicate that no index value is present for that test')
        if self.avm2:
            self.avm2DefaultName += ':'+self.avm2revision
            if self.iterations == 1:
                self.js_print('\n{0:>{1}}{2:>{3}}'.format(self.avmname, self.testFieldLen+self.resultWidth-2,
                                                             self.avm2name, self.resultWidth+1))
                self.js_print('%-*s %5s %7s %9s\n' % (self.testFieldLen, 'test', 'avg',
                                                        'avg', '%diff'))
            else:   # multiple iterations
                if self.detail:
                    # Original old-school header - deprecated
                    self.js_print('\n%-*s %-33s %-33s' % (self.testFieldLen, '', self.avmname, self.avm2name))
                    self.js_print('%-*s  %7s :%7s  %7s %6s    %7s :%7s  %7s %6s %7s %8s' % (self.testFieldLen, 'test', 'min','max','avg','stdev','min','max','avg','stdev','%diff','sig  '))
                    self.js_print('%*s ---------------------------------   ---------------------------------   -----  --------' % (self.testFieldLen, '') )
                elif pythonVersion26():
                    # python >= 2.6
                    self.js_print('')
                    self.js_print('{0:>{1}}{2:>{3}}'.format(self.avmname,self.testFieldLen+self.resultWidth*2,
                                                             self.avm2name, self.resultWidth*2))
                    self.js_print('{0:<{testwidth}}{1:>{rw}}{2:>{rw}}{3:>{rw}}{4:>{rw}}{5:>{rw}}{6:>{rw}}'.
                                  format('test', 'best', 'avg', 'best', 'avg', '%dBst', '%dAvg',
                                         testwidth=self.testFieldLen, rw=self.resultWidth))
                else:
                    # python <= 2.5
                    self.js_print('\n%-*s %-*s  %-*s' % (self.testFieldLen+self.resultWidth-4, '',
                                                         self.resultWidth*2, self.avmname,
                                                         self.resultWidth*2, self.avm2name))
                    self.js_print('%-*s %*s %*s %*s %*s %*s %*s' % (self.testFieldLen, 'test',
                                                                    self.resultWidth, 'best',
                                                                    self.resultWidth, 'avg',
                                                                    self.resultWidth, 'best',
                                                                    self.resultWidth, 'avg',
                                                                    self.resultWidth, '%dBst',
                                                                    self.resultWidth, '%dAvg'))
        else:   # only one avm
            if (self.iterations>1):
                self.js_print(('\n\n%-*s %6s %7s %12s\n') % (self.testFieldLen, 'test',                                                            
                                                        self.besttime,
                                                        'avg','95%_conf'))
            else:
                self.js_print("\n\n%-*s %7s \n" % (self.testFieldLen, "test", "result"))


    def runTests(self, testList):
        testnum = len(testList)
        for t in testList:
            testnum -= 1
            o = self.runTest((t, testnum))

    def parseMemHigh(self, line):
        memoryhigh = 0
        tokens=line.rsplit()
        if len(tokens)>4:
            _mem=tokens[3]
            if _mem.startswith('('):
                _mem=_mem[1:]
            if _mem.endswith(')'):
                _mem=_mem[:-1]
            if _mem.endswith('M'):
                val=float(_mem[:-1])*1024
            else:
                val=float(_mem[:-1])
            if val>memoryhigh:
                memoryhigh=val
        return memoryhigh

    def loadTestSettings(self, dir, testname):
        settings = {}
        includes = self.includes #list

        # get settings for this test (from main testconfig file loaded into self.settings)
        for k in self.settings.keys():
            if re.search('^'+k+'$', testname):
                for k2 in self.settings[k].keys():
                    if k2 in settings:
                        settings[k2].update(self.settings[k][k2])
                    else:
                        settings[k2] = self.settings[k][k2].copy()


        if isfile(join(dir,self.testconfig)):
            localIncludes, localSettings = self.parseTestConfig(dir)
            # have a local testconfig, so we create a copy of the global settings to not overwrite
            includes = list(self.includes) #copy list - don't use reference
            includes.extend(localIncludes)
            if testname in localSettings:
                settings.update(localSettings[testname])

        return settings, includes

    def parsePerfTestOutput(self, output, resultDict):
        '''Parse the given lines of output for test results'''
        if self.debug:
            print(output)

        if self.memory:
            memoryhigh = 0
            for line in output:
                if '[mem]' in line and 'mmgc' in line:
                    tempmem = self.parseMemHigh(line)
                    if tempmem > memoryhigh:
                        memoryhigh = tempmem
            resultDict.setdefault('memory', []).append(memoryhigh)
            # only display memory unless user is asking for other metrics
            if not self.displayMetrics:
                return
            

        if self.perfm:
            # These metrics are NOT prefaced with the metric keyword
            for line in output:
                result = line.strip().split(' ')[-2]
                if 'verify & IR gen' in line:
                    resultDict.setdefault('vprof-verify-time', []).append(int(result))
                elif 'code ' in line:
                    resultDict.setdefault('vprof-code-size', []).append(int(result))
                elif 'compile ' in line:
                    resultDict.setdefault('vprof-compile-time', []).append(int(result))
                elif ('IR-bytes' in line) or ('mir bytes' in line):
                    resultDict.setdefault('vprof-ir-bytes', []).append(int(result))
                elif ('IR ' in line) or ('mir ' in line): #note trailing space
                    resultDict.setdefault('vprof-ir-time', []).append(int(result))
                    resultDict.setdefault('vprof-count', []).append(int(line.strip().split(' ')[-1]))

        # get all other metrics displayed
        for line in output:
            # If the testcase failed validation then stop parsing
            if 'validation failed' in line.lower():
                break

            # results must have the form of 'metric metric_name value'
            if 'metric' in line:
                rl=line.rsplit()
                if self.displayMetrics and 'all' not in self.displayMetrics:
                    # need to check which metrics to display
                    if rl[1].strip() not in self.displayMetrics:
                        continue
                if len(rl)>2:
                    if '.' in rl[2]:
                        resultDict.setdefault(rl[1], []).append(float(rl[2]))
                    else:
                        resultDict.setdefault(rl[1], []).append(int(rl[2]))

    def calculateSpeedup(self, testName, resultDict, resultDict2):
        '''calculate speed diff between vms
            stores all information into self.testData
        '''
        # TODO cpeyer: I'm still not certain that the correct thing to do re: index is to
        # immediately change the raw values to indexed values

        # testData structure:
        # { testName : { metric : { results1/2 : [], best1/2 : num, avg1/2 : num, spdup : num }}}

        # calc values for each metric in resultDict and resultDic2 if defined
        if resultDict2:
            # it's possible that there are different metrics being reported by each vm
            # only calculate the common metrics (intersection of the dict keys)
            metrics = set(resultDict.keys()) & set(resultDict2.keys())
        else:
            metrics = resultDict.keys()
        for metric in metrics:
            # if using the index file, compute the indexes and use those as results instead of raw values
            if self.indexFile and not self.saveIndex:
                resultDict[metric] = [self.computeIndex(testName, metric, x) for x in resultDict[metric]]
            # Store the results
            self.testData.setdefault(testName, {}).setdefault(metric, {}).setdefault('results1', []).extend(resultDict[metric])
            # calculate the best result
            try:
                r1 = self.testData[testName][metric]['best1'] = self.metricInfo[metric]['best'](resultDict[metric])
            except KeyError:
                # metric is not defined in metricinfo, default to using min
                r1 = self.testData[testName][metric]['best1'] = min(resultDict[metric])
                # also add the metric to metricinfo
                self.metricInfo[metric] = {'best':min, 'largerIsFaster':False}
            a1 = self.testData[testName][metric]['avg1'] = mean(resultDict[metric])

            if self.logresults:
                self.socketlog("addresult2::%s::%s::%s::%0.1f::%s::%s::%s::%s::%s::%s::%s;" %
                                   (testName, metric, r1, conf95(resultDict[metric]),
                                    self.testData[testName][metric]['avg1'],
                                    len(resultDict[metric]),
                                    self.osName.upper(), self.log_config,
                                    self.avmrevision, self.vmname, self.repo))

            if self.score:
                self.updateScore(self.score1, metric, r1)

            if resultDict2:
                if self.indexFile and not self.saveIndex:
                    resultDict2[metric] = [self.computeIndex(testName, metric, x) for x in resultDict2[metric]]
                # Store the results
                self.testData.setdefault(testName, {}).setdefault(metric, {}).setdefault('results2', []).extend(resultDict2[metric])
                # calculate the best result
                r2 = self.testData[testName][metric]['best2'] = self.metricInfo[metric]['best'](resultDict2[metric])
                a2 = self.testData[testName][metric]['avg2'] = mean(resultDict2[metric])
                if self.score:
                    self.updateScore(self.score2, metric, r2)
                # calculate speedup btwn vms
                # if the best value is the max value, reverse the sign of the spdup
                sign = -1 if self.metricInfo[metric]['largerIsFaster'] else 1
                self.testData[testName][metric]['spdup'] = 0 if r1 == 0 else sign * float(r1-r2)/r1 * 100.0
                self.testData[testName][metric]['avg_spdup'] = 0 if a1 == 0 else sign * float(a1-a2)/a1 * 100.0

    def checkForMetricChange(self, metric):
        ''' If the test metric has changed, print out a line indicating so.
            Function only used when displaying results sorted by metric.
            (Or only a single metric is being displayed)
        '''
        if self.currentMetric != metric:
            self.currentMetric = metric
            self.js_print('Metric: %s%s %s' % (self.metricInfo[metric].get('name',metric),
                                                ' (indexed)' if self.indexFile else '',
                                                self.metricInfo[metric].get('desc','')))

    def getBestResult(self, metric, resultList):
        return max(resultList) if self.metricInfo[metric]['largerIsFaster'] else min(resultList)

    def getSigString(self, sig, spdup):
        'generate a string of +/- to give a quick visual representation of the perf difference'
        return '--' if (sig < -2.0 and spdup < -5.0) else '- ' if sig < -1.0 \
            else '++' if (sig > 2.0 and spdup > 5.0) else '+ ' if sig > 1.0 else '  '

    def computeIndex(self, testname, metric, value):
        'Compute the index value for given testname and index from self.testIndexDict'
        indexValue = self.testIndexDict.get(testname, {}).get(metric)
        if indexValue:
            return (float(value)/indexValue)
        else:
            return -1

    def truncateDescField(self, desc):
        'Return desc truncated to self.testFieldLen'
        if desc.endswith('.as'):
            desc = desc[:-3]
        return desc if len(desc) <= self.testFieldLen else desc[(len(desc) - self.testFieldLen):]

    def updateScore(self, scoreDict, metric, result):
        'Update the given score dict with newest result'
        if not metric in scoreDict:
            scoreDict[metric] = {'score':float(result), 'count':1}
        else:
            scoreDict[metric] = {'score':scoreDict[metric]['score'] * float(result),
                                       'count':scoreDict[metric]['count']+1}

    def printScoreSummary(self):
        print('Score for %s:' % (self.avmname))
        for k,v in self.score1.iteritems():
            print('  %s = %s' % (k, str(pow(v['score'],1.0/v['count']))))
        if self.score2:
            print('Score for %s' % (self.avm2name,))
            for k,v in self.score2.iteritems():
                print('  %s = %s' % (k, str(pow(v['score'],1.0/v['count']))))
    '''
    def formatResult(self, result, truncateLen=DEFAULT_TRUNCATE_LEN, sigFigs = 1, metric = ''):
        #Format the test result for display
        # use currentMetric if no metric specified
        metric = metric or self.currentMetric
        if self.indexFile:
            # automatically format as 2 sigfigs float unless its an int
            if int(result) == result:
                return int(result)
            else:
                return ('%% %s.%sf' % ((truncateLen, 2))) % result
        if metric == 'memory':
            return formatMemory(result)
        else:
            if int(result) == result or abs(result) > 10**(truncateLen-1):
                return int(result)
            else:
                return ('%% %s.%sf' % ((truncateLen, sigFigs))) % result
                # line below requires python >= 2.6
                #return format(result, '%s.%sf' % (truncateLen, decimalPlaces))
    '''
    def formatResult(self, result, truncateLen=DEFAULT_TRUNCATE_LEN, sigFigs = None, metric = ''):
        if sigFigs is None:
            # Infer suitable number of decimal places from result's magnitude
            if result < 1:
                sigFigs = 3
            elif result < 10:
                sigFigs = 2
            else:
                sigFigs = 1

        #Format the test result for display
        # use currentMetric if no metric specified
        metric = metric or self.currentMetric
        if self.indexFile:
            # automatically format as 2 sigfigs float unless its an int
            return ('%% %s.%sf' % ((truncateLen, 2))) % result
        if metric == 'memory':
            return formatMemory(result)
        else:
            return ('%% %s.%sf' % ((truncateLen, sigFigs))) % result
                # line below requires python >= 2.6
                #return format(result, '%s.%sf' % (truncateLen, decimalPlaces))

    def runTest(self, testAndNum):
        'Run a singe performance testcase self.iterations times and print out results'
        ast = testAndNum[0]
        testName = ast

        # strip off ./ as test is then treated differently in perf db
        if testName[:2] == './':
            testName = testName[2:]
        if self.altsearchpath!=None and ast.startswith(self.altsearchpath):
            testName = ast[len(self.altsearchpath):]
        testnum = testAndNum[1]

        if ast.startswith("./"):
            ast=ast[2:]
        dir =ast[0:ast.rfind('/')]
        root,ext = splitext(ast)
        tname = root[root.rfind('/')+1:]
        abc = "%s.abc" % root

        settings = self.get_test_settings(root)

        if '.*' in settings and 'skip' in settings['.*']:
            self.verbose_print('  skipping %s' % testName)
            self.allskips += 1
            return

        if self.forcerebuild and isfile(abc):
            os.unlink(abc)
        if isfile(abc) and getmtime(ast)>getmtime(abc):
            self.verbose_print("%s has been modified, recompiling" % ast)
            os.unlink(abc)
        if not isfile(abc):
            self.compile_test(ast)
            if not isfile(abc):
                self.js_print("compile FAILED!, file not found " + abc)

        # determine current config
        config = "%s" % self.vmargs.replace(" ", "")
        config = "%s" % config.replace("\"", "")
        if config.find("-memlimit")>-1:
            config=config[0:config.find("-memlimit")]

        # results are stored in a dictionary using the metric as key
        # e.g.: {'time':[1,2,3,4]}
        resultsDict1 = {}
        resultsDict2 = {}
        if self.memory and self.vmargs.find("-memstats")==-1:
            self.vmargs="%s -memstats" % self.vmargs
        if self.memory and len(self.vmargs2)>0 and self.vmargs2.find("-memstats")==-1:
            self.vmargs2="%s -memstats" % self.vmargs2

        scriptargs=[['','']]
        if os.path.exists("%s.script_args" % testName):
            lines=open("%s.script_args" % testName).read().split('\n')
            scriptargs=[]
            for line in lines:
                if len(line.strip())==0 or line.strip().startswith('#'):
                    continue
                tokens=line.split(',')
                args=tokens[0]
                if args.startswith('-- ')==False:
                    args='-- %s' % args
                if len(tokens)>0:
                    desc=tokens[1]
                else:
                    desc=args.replace(' ','_')
                scriptargs.append([args,desc])
            if len(scriptargs)==0:
                scriptargs=['','']

        for arg in scriptargs:
            scriptArg=arg[0]
            testNameDesc=testName+arg[1]
            resultsDict1 = {}
            resultsDict2 = {}
            for i in range(self.iterations):
                if self.aotsdk and self.aotout:
                    progname = testName.replace(".as", "")
                    progname = progname.replace("/", ".")
                    progpath = os.path.join(self.aotout, progname)
                    if not self.avm:
                        (f1,err,exitcode) = self.run_pipe(os.path.join(self.aotout, progname))
                        # print("about to execute: " + os.path.join(self.aotout, progname))
                        exitcode = 0 # hack!
                    elif self.avm: # AVM is set to a script that will handle SSH communications
                        cmd = "%s %s %s %s" % (self.avm, progpath, self.vmargs, scriptArg)
                        (f1,err,exitcode) = self.run_pipe(cmd)
                        self.debug_print("%s" % (cmd))
                        self.debug_print(f1)
                        if testName in self.aot_compile_times:
                            f1.append('metric compile_time %.2f' % self.aot_compile_times[testName])
                else:
                    (f1,err,exitcode) = self.run_pipe("%s %s %s %s" % (self.avm, self.vmargs, abc, scriptArg))
                    self.debug_print("%s %s %s %s" % (self.avm, self.vmargs, abc, scriptArg))
                    self.debug_print(f1)
                if self.avm2:
                    (f2,err2,exitcode2) = self.run_pipe("%s %s %s %s" % (self.avm2, self.vmargs2 if self.vmargs2 else self.vmargs, abc, scriptArg))
                    self.debug_print("%s %s %s %s" % (self.avm2, self.vmargs2 if self.vmargs2 else self.vmargs, abc, scriptArg))
                    self.debug_print(f2)
                try:
                    if exitcode!=0:
                        self.finalexitcode=1
                        self.js_print("%-50s %7s %s" % (testName,'Avm1 Error: Test Exited with exit code:', exitcode))
                        return
                    else:
                        self.parsePerfTestOutput(f1, resultsDict1)
                    if self.avm2:
                        if exitcode2!=0:
                            self.finalexitcode=1
                            self.js_print("%-50s %7s %s" % (testName,'Avm2 Error: Test Exited with exit code:', exitcode))
                            return
                        else:
                            self.parsePerfTestOutput(f2, resultsDict2)
                except:
                    traceback.print_exc()
                    exit(-1)
            # end for i in range(iterations)

            if not self.validResultsDictionary('avm', resultsDict1, testName, f1):
                return
            if self.avm2 and not self.validResultsDictionary('avm2', resultsDict2, testName, f2):
                return
        
            # calculate best results and store to self.testData
            self.calculateSpeedup(testNameDesc, resultsDict1, resultsDict2)

            self.printTestResults(testNameDesc)
    
    def validResultsDictionary(self, avmName, resultsDict, testName, output):
        '''Make sure that the results dictionary has valid number of results for each metric
            Return True if valid, False if not valid
        '''
        if not resultsDict:
            self.js_print('%s : No metrics returned from test!' % testName)
            self.js_print('  test output: %s' % [l for l in output])
            return False
        # check to make sure every metric has the right number of results
        for metric in resultsDict.keys():
            if len(resultsDict[metric]) != self.iterations:
                self.js_print('%s : %s number of results for the %s metric is != # of iterations (%s)' %
                              (testName, avmName, metric, self.iterations))
                return False
        return True

    def checkForDirChange(self, name):
        # extract dir
        try:
            last_slash = name.rindex('/')+1
        except ValueError:
            last_slash = None
        
        if last_slash:
            dir = name[:last_slash]
            if not self.fullpath:
                name = '  '+name[last_slash:]
        
            if dir and dir != self.currentDir:
                self.js_print('Dir: %s' % dir)
                self.currentDir = dir
        
        return name
        
    
    def printTestResults(self, testName):
        '''Print the results for a single test'''
        # Support two output modes:
        # 1. Sorted by test, each metric gets a seperate line after each testname (default)
        # 2. Sorted by metric.  Output the results for a single metric (TODO: how do i determine?  command switch?)
        #    and then when the entire testrun is finished, output results for all other metrics

        testData = self.testData

        # How many metrics are stored for this test?
        numMetrics = len(testData[testName])

        if numMetrics == 1:
            metric = list(testData[testName].keys())[0]
            # print out metric info if needed
            self.checkForMetricChange(metric)

        # Print out dir names and indent tests below
        desc = self.checkForDirChange(testName)

        if self.avm2:
            if self.iterations == 1:
                if numMetrics == 1:
                    self.printSingleIterationComparison(desc, testName, metric)
                else:   # numMetrics > 1
                    self.js_print(desc)
                    for metric in testData[testName].keys():
                        self.printSingleIterationComparison('    %s' % self.metricInfo[metric].get('name',metric), testName, metric)
            else:   # multiple iterations
                if numMetrics == 1:
                    self.printMultiIterationComparison(desc, testName, metric)
                else: # numMetrics > 1
                    self.js_print(desc)
                    for metric in testData[testName].keys():
                        self.printMultiIterationComparison('    %s' % self.metricInfo[metric].get('name',metric), testName, metric)
        else: # only one avm tested
            if self.iterations == 1:
                if numMetrics == 1:
                    self.js_print('%-*s %*s' % (self.testFieldLen, self.truncateDescField(desc),
                                            self.resultWidth, self.formatResult(testData[testName][metric]['best1'], metric=metric)))
                else:   # numMetrics > 1
                    self.js_print(desc)
                    for metric in testData[testName].keys():
                        self.js_print('    %-*s %*s' % (self.testFieldLen-2, self.metricInfo[metric].get('name',metric),
                                            self.resultWidth, self.formatResult(testData[testName][metric]['best1'], metric=metric)))

            else:   # multiple iterations
                if numMetrics == 1:
                    self.js_print(('%-*s %*s %*s %4.1f%% %s') %                        
                        (self.testFieldLen, self.truncateDescField(desc), self.resultWidth,
                          self.formatResult(testData[testName][metric]['best1'], metric=metric),
                          self.resultWidth, self.formatResult(testData[testName][metric]['avg1'], metric=metric),
                          conf95(self.testData[testName][metric]['results1']),
                         [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results1']] if self.raw else ''
                         ))
                else:   # numMetrics > 1
                    self.js_print(desc)
                    for metric in testData[testName].keys():
                        self.js_print(('    %-*s %*s %*s %4.1f%% %s') %                            
                        (self.testFieldLen-2, self.metricInfo[metric].get('name',metric),
                         self.resultWidth,
                         self.formatResult(testData[testName][metric]['best1'], metric=metric),
                         self.resultWidth, self.formatResult(testData[testName][metric]['avg1'], metric=metric),
                         conf95(self.testData[testName][metric]['results1']),
                         [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results1']] if self.raw else ''
                         ))
            #else:
            #        self.js_print("%-*s %5s %s" % (self.testFieldLen, truncateTestname(testName),
            #                                       'no test result - test output: ',f1))
            #        self.finalexitcode=1

    def printSingleIterationComparison(self, descStr, testName, metric):
        '''Print output for single iteration when comparing 2 vms'''
        spdup = self.testData[testName][metric]['spdup']
        avg_spdup = self.testData[testName][metric]['avg_spdup']
        
        if pythonVersion26():
            self.js_print('{0:<{testwidth}}{1:>{rw}}{2:>{rw}}{3:>{rw}}{4:>{rw}}'.
                            format(self.truncateDescField(descStr),
                                   self.formatResult(self.testData[testName][metric]['best1'], metric=metric),
                                   self.formatResult(self.testData[testName][metric]['best2'], metric=metric),
                                   self.formatResult(spdup, 4, 1, 'percent'),
                                   self.formatResult(avg_spdup, 4, 1, 'percent'),
                                   testwidth=self.testFieldLen, rw=self.resultWidth))
        else: # python <= 2.5
            self.js_print('%-*s %5s %7s %6.1f %6.1f' % (self.testFieldLen, self.truncateDescField(descStr),
                                                          self.formatResult(self.testData[testName][metric]['best1'], metric=metric),
                                                          self.formatResult(self.testData[testName][metric]['best2'], metric=metric),
                                                          spdup, avg_spdup))
            
    def printMultiIterationComparison(self, descStr, testName, metric):
        '''Print output for multiple iterations when comparing 2 vms'''
        relStdDev1 = rel_std_dev(self.testData[testName][metric]['results1'])
        relStdDev2 = rel_std_dev(self.testData[testName][metric]['results2'])
        spdup = self.testData[testName][metric]['spdup']
        avg_spdup = self.testData[testName][metric]['avg_spdup']

        try:
            sig = spdup / (relStdDev1+relStdDev2)
        except ZeroDivisionError:
            # determine sig by %diff (spdup) only
            sig = cmp(spdup,0) * (3.0 if abs(spdup) > 5.0 else 2.0 if abs(spdup) > 1.0 else 0.0)
        sig_str = self.getSigString(sig, spdup)
        if self.detail:
            self.js_print('%-*s [%7s :%7s] %7s ±%4.1f%%   [%7s :%7s] %7s ±%4.1f%% %6.1f%% %6.1f %2s %s %s' %
                          (self.testFieldLen, self.truncateDescField(descStr),
                           self.formatResult(min(self.testData[testName][metric]['results1']), metric=metric),
                           self.formatResult(max(self.testData[testName][metric]['results1']), metric=metric),
                           self.formatResult(self.testData[testName][metric]['avg1'], metric=metric),
                           relStdDev1,
                           self.formatResult(min(self.testData[testName][metric]['results2']), metric=metric),
                           self.formatResult(max(self.testData[testName][metric]['results2']), metric=metric),
                           self.formatResult(self.testData[testName][metric]['avg2'], metric=metric),
                           relStdDev2,
                           spdup, avg_spdup,sig_str,
                           [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results1']] if self.raw else '',
                           [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results2']] if self.raw else ''
                           ))
        elif pythonVersion26():
            self.js_print('{0:<{testwidth}}{1:>{rw}}{2:>{rw}}{3:>{rw}}{4:>{rw}}{5:>{rw}}{6:>{rw}}{7:>3}{8}{9}'.
                            format(self.truncateDescField(descStr),
                                   self.formatResult(self.testData[testName][metric]['best1'], metric=metric),
                                   self.formatResult(self.testData[testName][metric]['avg1'], metric=metric),
                                   self.formatResult(self.testData[testName][metric]['best2'], metric=metric),
                                   self.formatResult(self.testData[testName][metric]['avg2'], metric=metric),
                                   self.formatResult(spdup, 4, 1, 'percent'),
                                   self.formatResult(avg_spdup, 4, 1, 'percent'),
                                   sig_str,
                                   [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results1']] if self.raw else '',
                                   [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results2']] if self.raw else '',
                                   testwidth=self.testFieldLen, rw=self.resultWidth))
        
        else:
            # python <= 2.5
            self.js_print('%-*s %*s %*s %*s %*s %*s %*s %2s %s %s' %
                          (self.testFieldLen, self.truncateDescField(descStr),
                           self.resultWidth, self.formatResult(self.testData[testName][metric]['best1'], metric=metric),
                           self.resultWidth, self.formatResult(self.testData[testName][metric]['avg1'], metric=metric),
                           self.resultWidth, self.formatResult(self.testData[testName][metric]['best2'], metric=metric),
                           self.resultWidth, self.formatResult(self.testData[testName][metric]['avg2'], metric=metric),
                           self.resultWidth, self.formatResult(spdup, 4, 2, 'percent'),
                           self.resultWidth, self.formatResult(avg_spdup, 4, 2, 'percent'),
                           sig_str,
                           [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results1']] if self.raw else '',
                           [self.formatResult(x, metric=metric) for x in self.testData[testName][metric]['results2']] if self.raw else ''
                           ))

    def convertAvmOptionsDictToList(self):
        '''Convert self.avmOptions to lists that can be used by csvwriter'''
        avmOptionsHeader = []
        avmOptions = []
        avm2Options = []
        
        # get all the keys from both option dictionaries
        # uniquify the list so there are no duplicates using set
        keys = sorted(set(self.avmOptionsDict.keys() + self.avm2OptionsDict.keys()))
        
        for key in keys:
            avmOptionsHeader.append(key)
            avmOptions.append(self.avmOptionsDict.get(key, False))
            avm2Options.append(self.avm2OptionsDict.get(key, False))
                        
        return avmOptionsHeader, avmOptions, avm2Options
            
    
    def outputCsvToFile(self):
        # testData structure:
        # { testName : { metric : { results1/2 : [], best1/2 : num, avg1/2 : num, spdup : num }}}
        import csv
        try:
            csvwriter = csv.writer(open(self.csvfile, 'a' if self.csvAppend else 'w'))
        except IOError:
            if self.csvfile != 'output.csv':
                print('Error attempting to open %s.  Saving to ./output.csv instead' % self.csvfile)
                self.csvfile = 'output.csv'
                self.outputCsvToFile()
            else:
                print('Error attempting to write to output.csv file - aborting.')
            return
        
        print('Writing out csv data to %s' % self.csvfile)
        
        # TODO: generation of the options dict needs to be moved to runtestBase
        # more work is needed to get that working there
        # This here is proof of concept that needs to be fleshed out
        self.avmOptionsDict = {
            'avm' : self.avmname,
        }
        
        self.avm2OptionsDict = {
            'avm' : self.avm2name,
        }
        
        avmOptionsHeader, avmOptions, avm2Options = self.convertAvmOptionsDictToList()
        
        # write the header
        csvwriter.writerow(avmOptionsHeader + ['testname', 'metric', 'metricunit', 'iteration', 'value'])
        
        # Write out all the data contained in self.testData to the csv file
        for testname, testDict in self.testData.iteritems():
            for metric, metricDict in testDict.iteritems():
                for iteration, value in enumerate(metricDict['results1']):
                    csvwriter.writerow(avmOptions + \
                        [testname, metric,
                         self.metricInfo[metric].get('unit', ''),
                         iteration+1, # use iteration+1 so that iteration values are not zero based
                         value])
                if metricDict.get('results2'):
                    for iteration, value in enumerate(metricDict['results2']):
                        csvwriter.writerow(avm2Options + \
                            [testname, metric,
                             self.metricInfo[metric].get('unit', ''),
                             iteration+1, # use iteration+1 so that iteration values are not zero based
                             value])

                    
    def outputTestIndexFile(self):
        '''write out a testIndex file'''
        output =  [self.testIndexHeaderString]

        # update testIndexDict with newest results
        for testname, testDict in self.testData.iteritems():
            for metric, metricDict in testDict.iteritems():
                self.testIndexDict.setdefault(testname, {}).update({metric:metricDict['best1']})
                
        output.append('testIndexDict = ')
        
        print('Saving values to index file: %s' % self.saveIndexFile)
        try:
            f = open(self.saveIndexFile, 'w')
        except IOError:
            print('Error attempting to open %s for write.' % self.saveIndexFile)
            print('Aborting saving of index file.')
            return
        
        f.writelines(output)

        import pprint
        pprint.pprint(self.testIndexDict, f)



try:
    runtest = PerformanceRuntest()
    exit(runtest.finalexitcode)
except SystemExit:
    raise
except TypeError:
    # This is the error thrown when ctrl-c'ing out of a testrun
    print('\nKeyboard Interrupt')
    raise
except:
    print('Runtest Abnormal Exit')
    raise
