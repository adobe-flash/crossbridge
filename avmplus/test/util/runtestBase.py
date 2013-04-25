#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
#
# simple script that executes tamarin certification tests.  you have to build the
# stand-alone, avmplus executable.
# see http://developer.mozilla.org/en/docs/Tamarin_Build_Documentation
#
# this test looks for an executable avmplus shell in
# %MOZ_SRC/mozilla/js/tamarin/platform/,
#

import os, sys, getopt, datetime, pipes, glob, itertools, tempfile, string, re, platform, shutil
import subprocess, random
from os.path import abspath, basename, dirname, exists, isdir, isfile, split, splitext, getmtime
from os import getcwd,environ
from datetime import datetime
from glob import glob
from sys import argv, exit
from getopt import getopt
from itertools import count
from time import time, sleep
import shutil
import traceback
from . import which
from . import threadpool
from . import convertAcceptanceToJunit

# runtestUtils must be imported after "from os.path import *" as walk is overridden
from . import runtestUtils
from .runtestUtils import walk,parseArgStringToList,TimeOutException,join, \
convertToCsv,detectCPUs,dict_match,formatMemoryList,formatMemory,list_match, \
parseArgStringToList,pPrint,splitList,genAtsArgs,moveAtsSwf,conf95,mean, \
rel_std_dev,standard_deviation,tDist,variance,getSignalName,signalNames, \
search_file, create_ats_swfversion_copy


# For Python 2.6 and above, use native subprocess.Popen
if sys.version_info[0] >= 3 \
    or (sys.version_info[0] == 2 and sys.version_info[1] >= 6):
    from subprocess import Popen
else:
    from killableprocess import Popen

if platform.system() == 'Windows':
    # pexpect only works on *nix systems
    pexpect_module_present = False
else:
    if sys.version_info[0] >= 3 \
        or (sys.version_info[0] == 2 and sys.version_info[1] >= 6):
        from . import pexpect
    else:
        import pexpect25 as pexpect
    pexpect_module_present = True


class RuntestBase(object):
    REGEX_CHARS = '( ) . * ? | [ ] \\'.split()
    
    abcOnlyExt = '.abc_' # only run, don't compile these abc files - underscore is used so that tests are not deleted when removing old abc files
    abcasmExt = '.abs'
    abcasmRunner = 'bash ../../utils/abcasm/abcasm.sh'
    abcasmShell = 'abcasm/abs_helper'
    abcdump = '../../utils/abcdump'
    addtoconfig = ''
    asc = ''
    ascargs = ''
    ash = ''
    # list of args to remove defined using -no-argname format in --ascargs
    asc_negative_args = []
    atsDir = 'ATS_SWFS'
    ascOutputFilename = 'ascoutput.log'
    ascOutputFile = None
    ascversion = ''
    ascbuild= ''
    avm = ''
    avmce = ''
    avmversion = ''
    avm_features = ''
    builtinabc = ''
    config = ''
    junitlog = None
    junitlogname = None
    
    # directives are used in the config files to indicate special test behavior
    directives = None
    # custom_directives will only run when the directive name is defined in the
    # config string (e.g: x86-mac-tvm-release-deep)
    custom_directives = ['deep', 'performance']
    # note that include is only valid for performance runtests config
    config_directives = ['expectedfail', 'skip', 'ats_skip', 'verify_skip',
                         'include', 'exclude'] + custom_directives
    
    
    escbin = ''
    failconfig = 'failconfig.txt'
    includes = None
    java = 'java'
    javaargs = ''
    js_output = ''
    logFileType = 'html'
    options = ''
    osName = ''
    playerglobalabc = ''
    settings = None
    shellabc = ''
    sourceExt = '.as'
    supportFolderExt = '_support'
    testconfig = 'testconfig.txt'
    test_harness_dir = '.'
    vmargs = ''
    vmtype = ''
    aotsdk = None
    aotout = None
    remoteip = None
    remoteuser = None
    aotextraargs = ""

    args = []
    ashErrors = []
    # ats template code
    atstemplate = [
        'import flash.display.*;\n',
        'import flash.util.*;\n',
        'var fileName:String ="_";\n', # this line will get replaced with the actual filename
        'this[fileName] = new Array();\n',
        'this[fileName+"Str"] = new Array();\n',
        'this[fileName+"Ans"] = new Array();\n'
        ]
    currentPids = []
    exclude = []
    executableExtensions = ()    # file extentions for tests that do not get compiled
    harness_abcs = []
    otherTestExtensions = ()     # other file extensions that are valid tests
    longOptions = []
    support_dirs = []   # list of support directories used when running --rebuildtests
    tests = []
    tests_no_run_re = []   # list of tests (possibly re) that will not be executed
    verified_executables = set([])
    winceProcesses = []

    ascOutputOnly = False
    csv = False
    cygwin = False
    debug = False
    eval = False      # Run the source file (.as, .js) but, do not magically prepend included files
    forcerebuild = False
    genAtsSwfs = False
    htmlOutput = False
    quiet = False
    random = False
    rebuildtests = False
    cleanexit = False
    runSource = False # Run the source file (.as, .js) instead of .abc, magically prepend included files
    show_time = False
    summaryonly = False
    timestampcheck = True
    timestamps = True
    useShell = True
    verbose = False
    verify = False
    writeResultProperties = False   # used by the asc runtests.py to write to a result.properties file used by the asc ant scripts
    valgrind = False

    randomSeed = None # can be specified so that random run runs in same order
    start_time = None
    testTimeOut = -1 #by default tests will NOT timeout
    threads = 1
    timeout = 0 # in seconds
    timeoutStartTime = 0
    

    lock = threadpool.threading.Lock()  # global lock used when threading

    def __init__(self):
        # Result Vars
        self.allpasses=0
        self.allfails=0
        self.allunpass=0
        self.allexpfails=0
        self.allexceptions=0
        self.allskips=0
        self.alltimeouts=0
        self.allasserts=0
        self.failmsgs=[]
        self.expfailmsgs=[]
        self.unpassmsgs=[]
        self.timeoutmsgs=[]
        self.assertmsgs=[]
        self.altsearchpath=None

        self.run()

    def __str__(self):
        return 'RuntestBase'

    def run(self):
        '''Implement Me'''
        pass

    def usage(self, c):
        print('usage: %s [options] [tests]' % basename(argv[0]))
        print(' -v --verbose       enable additional output')
        print(' -E --avm           avmplus command to use')
        print(' -a --asc           compiler to use')
        print('    --ash           interactive compiler to use')
        print(' -g --globalabc     DEPRECATED but still works - use builtin.abc (used to be location of global.abc)')
        print(' -b --builtinabc    location of builtin.abc')
        print(' -s --shellabc      location of shell_toplevel.abc')
        print(' -x --exclude       comma separated list of directories to skip')
        print(' -h --help          display help and exit')
        print(' -t --notime        do not generate timestamps (cleaner diffs)')
        print(' -f --forcerebuild  force rebuild all test files')
        print(' -c --config        sets the config string [default OS-tvm]')
        print('    --addtoconfig   add string to default config')
        print(' -q --quiet         display minimum output during testrun')
        print(' -l --log           also log all output to given logfile')
        print('    --logjunit      log to junit output format')
        print('    --logjunitname  specify the toplevel name for to junit output (e.g. windows-hybrid-acceptance')
        print('    --valgrind      run tests under valgrind')
        print('    --summaryonly   only display final summary')
        print('    --rebuildtests  rebuild the tests only - do not run against VM')
        print('    --cleanexit     when rebuilding media exit with 0 exitcode even if compilation fails')
        print('    --showtimes     shows the time for each test')
        print('    --ascargs       args to pass to asc on rebuild of test files')
        print('                    can also pass in -no-Argname to remove arg')
        print('    --ascoutput     output the asc commands to ascoutput.log. Does not compile or run any tests.')
        print('    --vmargs        args to pass to vm')
        print('    --timeout       max time to run all tests')
        print('    --testtimeout   max time to let a test run, in sec (default -1 = never timeout)')
        print('    --html          also create an html output file')
        print('    --notimecheck   do not recompile .abc if timestamp is older than .as')
        print('    --java          location of java executable (default=java)')
        print('    --javaargs      arguments to pass to java')
        print('    --random        run tests in random order')
        print('    --seed          explicitly specify random seed for --random')
        print('    --aotsdk        location of the AOT sdk used to compile tests to standalone executables.')
        print('    --aotout        where the resulting binaries should be put (defaults to the location of the as file).')
        print('    --aotargs       any extra arguments to pass to compile.py.')
        print('    --writeresult   write out the result.properties file, used when called from an ant script.')
        



    def setOptions(self):
        '''set the valid command line options.
            When subclassing, call this method first, then append options to each list'''
        self.options = 'vE:a:g:b:s:x:htfc:dql:'
        self.longOptions = ['verbose','avm=','asc=','ash=','globalabc=','builtinabc=','shellabc=',
                   'exclude=','help','notime','forcerebuild','config=','ascargs=','vmargs=',
                   'aotsdk=', 'aotout=', 'aotargs=', 'remoteip=', 'remoteuser=',
                   'timeout=','testtimeout=', 'rebuildtests','cleanexit','quiet','notimecheck',
                   'showtimes','java=','html','random', 'seed=', 'playerglobalabc=', 'toplevelabc=',
                   'javaargs=', 'summaryonly', 'log=', 'valgrind', 'addtoconfig=',
                   'writeresult','logjunit=','logjunitname=', 'ascoutput'
                   ]

    def parseOptions(self):
        try:
            opts, self.args = getopt(argv[1:], self.options, self.longOptions )
        except:
            self.usage(2)

        if not self.args:
            self.args = ['.']
        for o, v in opts:
            if o in ('-v', '--verbose'):
                self.verbose = True
            elif o in ('-h', '--help'):
                self.usage(0)
            elif o in ('-E', '--avm'):
                self.avm = v
            elif o in ('-a', '--asc'):
                self.asc = v
            elif o in ('--ash'):
                self.ash = v
            elif o in ('-b', '--builtinabc', '-g', '--globalabc'):
                self.builtinabc = v
            elif o in ('-s', '--shellabc', '--toplevelabc'):
                self.shellabc = v
            elif o in ('--playerglobalabc',):
                self.playerglobalabc = v
            elif o in ('-x', '--exclude'):
                self.exclude += v.split(',')
                # remove any trailing /,\ and whitespace from exclude list
                self.exclude = list(map(lambda s: s.rstrip('\\/ '), self.exclude))
            elif o in ('-t', '--notime'):
                self.timestamps = False
            elif o in ('-f', '--forcerebuild'):
                self.forcerebuild = True
            elif o in ('-c', '--config'):
                self.config = v
            elif o in ('--addtoconfig',):
                self.addtoconfig = v
            elif o in ('--ascargs',):
                self.ascargs = v
            elif o in ('--ascoutput'):
                self.ascOutputOnly = True
                self.rebuildtests = True
                self.forcerebuild = True
                self.threads = 1
            elif o in ('--vmargs',):
                self.vmargs = v
            elif o in ('--ext',):
                self.sourceExt = v
            elif o in ('--timeout',):
                try:
                    self.timeout = int(v)
                    # Bugzilla 707907: timeout should not mean failure
                    # unless --testtimeout passed.  But, we also do
                    # not want long-running / infinitely-looping tests
                    # to hijack --timeout. Below is a hack workaround.
                    self.testTimeOut=int(v*20)
                except ValueError:
                    print('Incorrect timeout value: %s\n' % v)
                    self.usage(2)
            elif o in ('--testtimeout',):
                try:
                    self.testTimeOut=int(v)
                except ValueError:
                    print('Incorrect testtimeout value: %s\n' % v)
                    self.usage(2)
            elif o in ('-d',):
                # Note that this is not documented in the usage text.  This
                # should only be used for debugging the runtests scripts
                self.debug = True
            elif o in ('--rebuildtests',):
                self.rebuildtests = True
                self.forcerebuild = True
            elif o in ('--cleanexit',):
                self.cleanexit = True
            elif o in ('-q', '--quiet'):
                self.quiet = True
            elif o in ('--summaryonly',):
                self.summaryonly = True
                self.quiet = True
            elif o in ('-l', '--log'):
                self.js_output = v
                self.logFileType = 'txt'
                self.createOutputFile()
            elif o in ('--logjunit'):
                self.junitlog = v
                if self.js_output == '':
                    self.js_output = v+'.txt'
                    self.logFileType = 'txt'
                self.createOutputFile()
            elif o in ('--logjunitname'):
                self.junitlogname=v
            elif o in ('--valgrind',):
                self.valgrind = True
            elif o in ('--html',):
                self.htmlOutput = True
            elif o in ('--notimecheck',):
                self.timestampcheck = False
            elif o in ('--showtimes',):
                self.show_time = True
            elif o in ('--java',):
                self.java = v
            elif o in ('--javaargs',):
                self.javaargs = v
            elif o in ('--random',):
                self.random = True
            elif o in ('--seed',):
                try:
                    self.randomSeed = int(v)
                except ValueError:
                    print('Incorrect seed value: %s\n' % v)
                    self.usage(2)
            elif o in ('--aotsdk',):
                self.aotsdk = v
            elif o in ('--aotout',):
                self.aotout = v
            elif o in ('--aotargs',):
                self.aotextraargs = v
            elif o in ('--writeresult',):
                self.writeResultProperties = True

        return opts


    ### Config and Settings ###

    def determineConfig(self):
        # ================================================
        # Determine the configruation if it has not been
        # passed into the script:
        # {CPU_ARCH}-{OS}-{ASC}-{VERSION}-{VMSWITCH}
        # ================================================

        try:
            # Try and determine CPU architecture of the AVM, if it fails drop back to platform.machine()
            cputype = ''
            (f,err,exitcode) = self.run_pipe('file "%s"' % (self.avm))
            f = ' '.join(f).replace(self.avm, '');
            self.verbose_print('determineConfig: %s' % f)

            if re.search('\(console\) 32-bit', f):
                cputype='arm'
                self.osName='winmobile-emulator'
            elif (re.search('Bourne-Again shell script', f) or re.search('python script', f)) and re.search('android',self.avm):
                cputype='arm'
                self.osName='android'
            elif re.search('(Mach-O universal binary)', f):
                # multiple architectures
                machine = platform.machine()
                if re.search('(x86_64|ppc64)',f):
                    if machine == 'Power Macintosh':
                        cputype = 'ppc64'
                    elif machine == 'i386':
                        cputype = 'x64'
                elif re.search('(i386|ppc)',f):
                    if machine == 'Power Macintosh':
                        cputype = 'ppc'
                    elif machine == 'i386':
                        cputype = 'x86'
            # Need to do the SPARC check before the x86 check as the x86 check
            # will find the "32-bit" string and think that it is x86.
            elif re.search('(SPARC)', f):
                cputype='sparc'
            elif re.search('(ppc64)', f):
                cputype='ppc64'
            elif re.search('(ppc)', f):
                cputype='ppc'
            elif re.search('(mips)', f.lower()):
                cputype='mips'
            elif re.search('(arm)', f.lower()):
                cputype='arm'
            # Keep this before x86 since `file $AVM` contains 'ELF 32-bit' on our SH4 hardware:
            #   ELF 32-bit LSB executable, Hitachi SH, version 1 (SYSV),
            #   for GNU/Linux 2.6.30, dynamically linked (uses shared libs),
            #   for GNU/Linux 2.6.30, not stripped
            elif re.search('( SH,)', f):
                cputype='sh4'
            elif re.search('(32-bit|80386|i386)', f):
                cputype='x86'
            elif re.search('(64-bit|x86-64|x86_64|Mono/\.Net)', f):
                cputype='x64'
            if cputype == '':
                raise Exception()

        except:
            try:
                cputype={'AMD64':'x86','x86':'x86','i386':'x86','i686':'x86','x86_64':'x64','i86pc':'x86','Power Macintosh':'ppc','sun4u':'x86','mips':'mips','armv7l':'arm','sh4':'sh4','':'x86'}[platform.machine()]
                if cputype == 'x86' and splitext(self.avm)[0][-2:] == '64':
                    cputype == 'x64'
            except:
                print("ERROR: cpu_arch '%s' is unknown, expected values are (x86,ppc), use runtests.py --config x86-win-tvm-release to manually set the configuration" % (platform.machine()))
                exit(1)

        # now that we've run file on the vm, prepend valgrind
        if self.valgrind:
            self.avm = 'valgrind --error-exitcode=1 --log-file=/tmp/valgrind-%p.txt ' + self.avm
            
        self.vmtype = 'release'
        if self.osName=='winmobile-emulator':
            # try to determine vmtype by filename
            vm_name = splitext(split(self.avm)[1])[0]
            if '_d' in vm_name:
                self.vmtype = 'debug'
            else:
                self.vmtype = 'release'
        elif not self.runSource and not self.rebuildtests:
            (f,err,exitcode) = self.run_pipe('%s -Dversion' % self.avm)
            try:
                self.avm_features = f[1]    # save feature list
                # determine avmshell type
                if re.search('debug-debugger',f[0]):
                    self.vmtype = 'debugdebugger'
                elif re.search('release-debugger',f[0]):
                    self.vmtype = 'releasedebugger'
                elif re.search('debug',f[0]):
                    self.vmtype = 'debug'
                elif re.search('release',f[0]):
                    self.vmtype = 'release'
                else:   # try to determine vmtype by filename
                    vm_name = splitext(split(self.avm)[1])[0]
                    if '_sd' in vm_name:
                        self.vmtype = 'debugdebugger'
                    elif '_s' in vm_name:
                        self.vmtype = 'releasedebugger'
                    elif '_d' in vm_name:
                        self.vmtype = 'debug'
                    else:
                        self.vmtype = 'release'

                # get the build number and hash
                self.avmversion = self.getAvmVersion(txt=f[0])

                # need to check for whether valgrind is running
                if re.search('valgrind',f[0]):
                    self.vmtype += '-valgrind'
            except:
                # Error getting shell info
                self.vmtype = 'unknown'
                self.avmversion = 'unknown'

        # need to also check for wordcode in the avm name for winmo
        if re.search('AVMFEATURE_WORDCODE_INTERP', self.avm_features) \
           or re.search('wordcode', self.avm):
            wordcode = '-wordcode'
        else:
            wordcode = ''
        asc_str="asc1"
        if self.ascversion:
            asc_str="asc%s" % self.ascversion
            
        self.config = cputype+'-'+self.osName+'-'+asc_str+'-'+self.vmtype+ \
                      wordcode+self.vmargs.replace(" ", "")+self.addtoconfig
        
        

    def determineOS(self):
        _os = platform.system()
        ostype = ''
        # When running on a windows system we can either be running with cygwin python
        # or with a windows-native python.  Legacy code requires that ostype be kept
        # as win for the osName, but one can check for the python type by querying
        # the self.cygwin boolean:
        # self.osName='win' and not self.cygwin == windows python
        # self.cygwin = cygwin python

        if re.search('(CYGWIN_NT)', _os):
            ostype='win'
            self.cygwin = True
        if re.search('(Windows)', _os):
            ostype='win'
            self.useShell = False
        if re.search('(Darwin)', _os):
            ostype='mac'
        if re.search('(Linux)', _os):
            ostype='lnx'
        if re.search('(SunOS)', _os):
            ostype='sol'

        if ostype == '':
            print("ERROR: os %s is unknown, expected values are (win,mac,lnx,sol), use runtests.py --config x86-win-tvm-release to manually set the configuration" % (platform.system()))
            exit(1)

        self.osName = ostype

    def loadPropertiesFile(self):
        # yet another way to specify asc,avm,builtinabc ...from a file
        pf = 'runtests.properties'
        if exists(pf):
            self.verbose_print( 'reading properties from %s' % (pf) )
            fd = open(pf,'r')
            for l in fd:
                setting = l.strip().split('=')
                if l.startswith('#') or len(setting) < 2 or len(setting[1]) <= 0:
                    continue
                val = setting[1].strip()
                option = setting[0].split('.')  # see if we have x.y = z
                nm = option[0].strip()
                # check if nm is valid
                if nm in self.__class__.__dict__ and not callable(self.__class__.__dict__[nm]):
                    if len(option) > 1:
                        val = self.__class__.__dict__[nm] + ' ' + val  # concat
                    self.__class__.__dict__[nm] = val
            fd.close()

    def setEnvironVars(self):
        if 'ASC' in environ:
            self.asc = environ['ASC'].strip()
        if 'ASH' in environ:
            self.ash = environ['ASH'].strip()
        if 'ASCARGS' in environ:
            self.ascargs = environ['ASCARGS'].strip()
        if 'AVM' in environ:
            self.avm = environ['AVM'].strip()
        if 'BUILTINABC' in environ:
            self.builtinabc = environ['BUILTINABC'].strip()
        if 'CONFIG' in environ:
            self.config = environ['CONFIG'].strip()
        if 'CVS' in environ:
            self.exclude = ['CVS'].strip()
        if 'GLOBALABC' in environ:  # deprecated var - users should use BUILTINABC
            self.builtinabc = environ['GLOBALABC'].strip()
        if 'PLAYERGLOBALABC' in environ:
            self.playerglobalabc = environ['PLAYERGLOBALABC'].strip()
        if 'SHELLABC' in environ:
            self.shellabc = environ['SHELLABC'].strip()
        if 'TOPLEVELABC' in environ:    # deprecated var - users should use SHELLABC
            self.shellabc = environ['TOPLEVELABC'].strip()
        if 'VMARGS' in environ:
            self.vmargs = environ['VMARGS'].strip()

    ### File and Directory functions ###
    
    def check_harness_files(self):
        # determine required test harness files
        harness_as_files = glob(join(self.test_harness_dir, '*.as'))
        for as_file in harness_as_files:
            abc = as_file[:-2] + 'abc'
            if not exists(abc) or (getmtime(as_file) > getmtime(abc)):
                # compile the abc
                cmd = ''
                if self.asc.endswith('.jar'):
                    cmd = self.java
                    cmd += ' -jar %s' %  self.asc
                else:
                    cmd = self.asc
                cmd += ' -import %s' %self.builtinabc
                self.run_pipe('%s %s' % (cmd,as_file))
            self.harness_abcs.append(abc)

    def build_incfiles(self, as_file):
        files=[]
        (dir, file) = split(as_file)
        (testdir, ext) = splitext(as_file)
        for util in glob(join(testdir,'*'+self.sourceExt)):
            files.append(util.replace("$", "\$"))
        return files

    def checkExecutable(self, exe, msg):
        if exe:
            exe = exe.split()[0]    # only check first arg if there are more than one
        # check to see if we've already checked this
        if exe in self.verified_executables:
            return
        
        original_exe = exe
        if not isfile(exe):
            # exe might be in path - check there
            try:
                if not which.which(exe):
                    exit('ERROR: cannot find %s, %s' % (exe, msg))
                exe = which.which(exe)
            except which.WhichError:
                exit('ERROR: cannot find %s, %s' % (exe, msg))
        if not os.access(exe, os.X_OK):
            # check in path
            try:
                if not os.access(which.which(exe)[0], os.X_OK):
                    try:
                        import stat
                        os.chmod(exe, stat.S_IXUSR)
                    except:
                        exit('ERROR: cannot execute %s, check the executable flag' % exe)
            except which.WhichError:
                exit('ERROR: cannot execute %s, check the executable flag' % exe)
        # add to verified_executables
        self.verified_executables.add(original_exe)

    def checkPath(self,additionalVars=[]):
        '''Check to see if running using windows python and if so, convert any cygwin paths to win paths
            Takes additional variables to check as a list of strings
        '''
        self.determineOS()
        if self.osName == 'win':
            def convertFromCygwin(cygpath):
                if cygpath.find('\\') == -1:
                    try:
                        f = self.run_pipe('cygpath -m %s' % cygpath)
                        cygpath = ' '.join([i.strip() for i in f[0]])
                    except:
                        pass
                return cygpath

            selfVarsToCheck = ['avm','asc','builtinabc','shellabc','java']
            selfVarsToCheck.extend(additionalVars)
            for var in selfVarsToCheck:
                setattr(self, var, convertFromCygwin(getattr(self,var)).strip())

            newargs = []
            for t in self.args:
                newargs.append(convertFromCygwin(t).strip())
            self.args = newargs

        if not self.rebuildtests and self.aotsdk==None:
            self.checkExecutable(self.avm, 'AVM environment variable or --avm must be set to avmplus')

    def createOutputFile(self):
        # set the output file name.  let's base its name on the date and platform,
        # and give it a sequence number.
        now = datetime.today()
        if not self.js_output:
            for i in count(1):
                self.js_output = '%d-%s-%s.%d.%s' % (now.year, str(now.month).zfill(2), str(now.day).zfill(2), i, self.logFileType)
                if not isfile(self.js_output):
                    break

        print('Writing results to %s' % self.js_output)
        js_output_f = open(self.js_output, 'w')
        js_output_f.close()

    def getTestsList(self, startDir):
        '''Get all possible tests to run, then parse it down depending on
            configuration.  Returns list of tests to run'''
        # Gather the list of all possible tests
        fileExtentions = (self.sourceExt,) + self.executableExtensions + self.otherTestExtensions
        if self.altsearchpath!=None:
            newstartDir=[]
            for a in startDir:
                newstartDir.append(self.altsearchpath+a)
            startDir=startDir+newstartDir
        for i in range(len(startDir)):
            if startDir[i][-1] == '/':
                startDir[i] = startDir[i][:-1]
            if startDir[i].startswith('./'):
                startDir[i] = startDir[i][2:]
        tests = [a for a in startDir if isfile(a) and self.istest(a, fileExtentions)]
        for a in [d for d in startDir if isdir(d) and not (basename(d) in self.exclude)]:
            for d, dirs, files in walk(a, followlinks=True):
                if d.startswith('./'):
                    d=d[2:]
                tests += [(d+'/'+f) for f in files if self.istest(f, fileExtentions)]
                # utilDirs contains all dirs that hold support files, and therefore
                # are excluded from the tests list
                # There are three kinds of util directories:
                # 1. directory with the same name as the test: all files in that dir
                #    are included when compiling the test
                # 2. directory with the same name as the test + _support (string is defined in self.supportFolderExt):
                #    all files in that dir are compiled, but not run - these files are
                #    normally passed in as args to the test itself
                # 3. directory with the name "includes": all files in that dir
                #    are manually included in test media and will not be compiled or run
                utilDirs = [ud for ud in dirs if (ud+self.sourceExt in files) or
                         (ud.endswith(self.supportFolderExt) and
                          ud[:-len(self.supportFolderExt)]+self.sourceExt in files)
                         or ud=="includes"]
                for x in [x for x in self.exclude+utilDirs if x in dirs]:
                    dirs.remove(x)
                    if x.endswith(self.supportFolderExt):
                        self.support_dirs.append(join(d, x))
        tests=[a for a in tests if os.path.dirname(a)!='' and os.path.dirname(a)!='.']

        def filter_test_list(directive):
            # pare down the testlist to only tests that match directive
            test_list = []
            if directive in self.directives:
                for test in tests:
                    for test_re in self.directives[directive]:
                        if re.search(test_re, test):
                            if test not in test_list:
                                test_list.append(test)
            return test_list
        
        # check for special directive mode in config (deep, performance, etc)
        for custom_directive in self.custom_directives:
            if custom_directive in self.config:
                tests = filter_test_list(custom_directive)
                break
        else:   # not running in any custom directive mode
            # remove any custom_directives from current run
            custom_directives_re = []
            for custom_directive in self.custom_directives:
                if custom_directive in self.directives:
                    custom_directives_re.extend(self.directives[custom_directive])

            tests_to_remove = [] # Need to track tests to remove since if the test is removed
                                 # while iterating, there is no guarantee that all items will
                                 # be properly iterated if it is modified.
            for test in tests:
                for test_to_remove_re in custom_directives_re + self.tests_no_run_re:
                    if re.search(test_to_remove_re, test):
                        try:
                            tests_to_remove.append(test)
                        except ValueError:
                            pass # test already removed

            for test in tests_to_remove:
                try:
                    tests.remove(test)
                except ValueError:
                    pass # test already removed

        # We still use the include directive in performance tests
        if str(self) == 'PerformanceRuntest' and 'include' in self.directives:
            tests = filter_test_list('include')
        
        if not tests:
            self.js_print('No tests found!')
            self.js_print('Exiting ...')
            sys.exit(0)
            
        return tests

    def get_test_settings(self, root):
        # return settings for this test
        settings = {}
        for k in self.settings.keys():
            match = False
            if k == root:
                match = True
            elif any(regex_char in k for regex_char in self.REGEX_CHARS):
                # Do a regex search if there are regex chars in the key
                if re.search('^'+k+'$', root):
                    match = True
                
            if match:
                for k2 in self.settings[k].keys():
                    if k2 in settings:
                        settings[k2].update(self.settings[k][k2])
                    else:
                        settings[k2] = self.settings[k][k2].copy()
        return settings

    def istest(self,f, fileExtentions):
        return f.endswith(fileExtentions) and not f.endswith('Util'+self.sourceExt)

    def parents(self, d):
        '''return a generator of the current dir and all parent directories up
            to the test dir root'''
        while d != abspath(self.args[0]) and d != '':
            yield d
            d = dirname(d)
        yield '.'   # yield the test dir root

    def parseTestConfig(self, config_file):
        '''read a configuration file and parse it into the settings dictionary'''
        settings = {}
        directives = {}
        multi_line = ''
        line_num = 0

        # If the config file doesn't exist, just return
        if not isfile(config_file):
            return settings, directives

        def print_parse_error(error_type, file_name, line, msg):
            '''Print out parsing errors in config files'''
            print('%s Parsing configuration file %s at line %s:' %
                  (error_type, file_name, line))
            print(msg)
            if error_type == 'ERROR':
                exit(1)
            
        # the field format is:
        # 0. testname:testcase (testcase optional)
        # 1. include configurations
        # 2. exclude configurations
        # 3. directive
        # 4. comment
        conffile = open(config_file, 'r')
        for line in conffile.readlines():
            line_num += 1
            line = line.strip()
            if line.startswith('#') or not line:
                # skip comments and empty lines
                continue
            elif line.endswith('\\'):
                # multi-line entry, save and load the next line
                multi_line += line[:-1]
                continue
            elif multi_line:
                # last line of multi-line entry
                line = multi_line + line
                multi_line = ''
            # set maxsplit to 4 to not split on any commas in comments
            fields = line.split(',', 4)
            # clean up any field whitespace and assign to readable vars
            try:
                test, include_config, exclude_config, directive, comment = [s.strip() for s in fields]
            except ValueError:
                msg = '  %s has %s fields, expected 5 fields' % (fields[0].strip(), len(fields))
                print_parse_error('ERROR', config_file, line_num, msg)
            # Verify that a valid directive was defined
            directive = directive.lower()
            if directive not in self.config_directives:
                msg = '  Unrecognized directive: %s.  Valid directives are: %s' \
                      % (directive, ', '.join(self.config_directives))
                print_parse_error('ERROR', config_file, line_num, msg)
            # split the test name from testcase
            if ':' in test:
                test, testcase = [s.strip() for s in test.split(':', 1)]
            else:
                testcase = '.*'
            if test.endswith((self.sourceExt, self.abcOnlyExt, self.abcasmExt)):
                ext = test[test.rfind('.'):]
                msg = '  %s contains an unnecessary file extension %s' % (test, ext)
                print_parse_error('WARNING', config_file, line_num, msg)
                test = test[:test.rfind('.')]
            if not comment:
                print_parse_error('WARNING', config_file, line_num,
                                  '  No comment found for %s' % test)
            # only add settings for current config and only re.search if defined
            try:
                include_match = re.search(include_config, self.config) if include_config else None
                exclude_match = re.search(exclude_config, self.config) if exclude_config else None
            except re.error:
                print_parse_error('ERROR', config_file, line_num,
                                  '  regex is malformed: %s' % sys.exc_info()[1])
            if directive == 'performance' \
                and 'performance' not in self.config:
                # performance tests only run in performance mode
                if test not in self.tests_no_run_re:
                    self.tests_no_run_re.append(test)
            elif include_match and not exclude_match:
                # test config matches self.config
                settings.setdefault(test, {}).setdefault(testcase,{})
                settings[test][testcase][directive] = comment
                # also store test-level settings by directive
                if testcase == '.*':
                    directives.setdefault(directive, []).append(test)
        conffile.close()
        return settings, directives
        

    def parseRootConfigFiles(self):
        # Load any root .asc_args and .java_args files so they don' have to be
        # loaded over and over again when compiling tests

        # Loads root asc_args file and modifies arglist accordingly

        if isfile('./dir.asc_args'):  # load root dir.asc_args
            ascArgsList = parseArgStringToList(self.ascargs)
            # seperate out the negative args
            for arg in ascArgsList:
                if arg[0:3].lower() == '-no':
                    self.asc_negative_args.append(arg[3:])
                    # delete the -no arg from the list
                    ascArgsList.remove(arg)
            ascArgsList = self.parseAscArgs(ascArgsList, './dir.asc_args', './')
            self.ascargs = ' '.join(ascArgsList)


        # Loads root asc_args file and modifies arglist accordingly
        if isfile('./dir.java_args'):  # load root dir.java_args
            javaArgsList = parseArgStringToList(self.javaargs)
            javaArgsList = self.parseAscArgs(javaArgsList, './dir.java_args', './')
            self.javaargs = ' '.join(javaArgsList)


    ### Output / Printing functions ###

    def err_print(self, m):
        self.js_print(m, '<font color=#990000>', '</font><br/>')

    def fail(self, abc, msg, failmsgs):
        msg = msg.strip()
        self.err_print('   %s' % msg)
        failmsgs += ['%s : %s' % (abc, msg)]

    def js_print(self, m, start_tag='<p><tt>', end_tag='</tt></p>', overrideQuiet=False):
        # Print output
        if self.quiet and not overrideQuiet:
            sys.stdout.write('.')
            sys.stdout.flush()
        else:
            try:
                print(m)
            except:
                print('invalid characters in output')
            sys.stdout.flush()
        if self.js_output:
            try:
                js_output_f = open(self.js_output, 'a')
                if self.logFileType == 'html':
                    js_output_f.write('%s %s %s\n' % (start_tag, m, end_tag))
                else:
                    js_output_f.write('%s\n' % m)
            except:
                pass
            js_output_f.close()

    def printOutput(self,request, outputCalls=None):
        #execute the outputCalls
        if outputCalls:
            for call in outputCalls:
                try:
                    call[0](*call[1])
                except TypeError:
                    # intermittent issue: TypeError: verbose_print() takes at most 4 arguments (53 given)
                    # see https://bugzilla.mozilla.org/show_bug.cgi?id=564124
                    print(sys.exc_info())
                    print('If you see this, please send the exception info above to actionscriptqe@adobe.com')

    def quiet_print(self, m, start=None, end=None):
        if self.quiet:
            sys.stdout.write('.')
        else:
            if not start and not end:
                self.js_print(m)
            else:
                self.js_print(m, start, end)

    ### The *ignored here is to mask a bug, https://bugzilla.mozilla.org/show_bug.cgi?id=564124

    def verbose_print(self, m, start='', end='', *ignored):
      if self.verbose:
        self.js_print(m, start, end)

    def debug_print(self, m, start='', end=''):
      if self.debug:
        self.js_print(m, start, end)

    ### ASC / Compiling Functions ###

    def compile_aot(self, abcfile, extraabcs = []):
        (testdir, ext) = splitext(abcfile)
        settings = self.get_test_settings(testdir)
        if '.*' in settings and 'skip' in settings['.*']:
            self.js_print('Skipping %s ... reason: %s' % (abcfile,settings['.*']['skip']))
            return ['', '', 0]

        output = os.path.dirname(abcfile)
        if self.aotout:
            output = self.aotout

        outname = abcfile.replace("./", "")
        # Replace this before .abc otherwise we won't be able to replace it
        outname = outname.replace(".abc_", "")
        outname = outname.replace(".abc", "")
        outname = outname.replace("/", ".")
        outabc = os.path.join(output, outname + ".abc")

        if not os.path.exists(output):
            os.mkdir(output)

        # Find additional abc files that are required @ runtime:
        source = abcfile
        if source.find(".abc_") == -1:
            source = source.replace(".abc", ".as")
        if exists(source+".avm_args"):
            avm_args_file = open('%s.avm_args' % source,'r')
            for line in avm_args_file:
                line = line.strip()
                if line.startswith('#'):
                    continue
                line, extraVmArgs, abcargs, multiabc = self.process_avm_args_line(line, os.path.dirname(source))
                break # AOT does not support runtime switches which could be multiple entries in this
                      # file, so only parse the first noncomment line to look for additional abc files to load
            avm_args_file.close()
            extraabcs = extraabcs + multiabc.split()

        if isfile(abcfile): # Safe guard
            shutil.copyfile(abcfile, outabc)
            while not os.path.exists(outabc): # Have had some IO issues with getting to the ADT call
                time.sleep(.5)                # before the file is actually copied over.
        copiedExtraAbcs=[]
        for abc in extraabcs:
            extraoutname = abc.replace("./", "")
            # Replace this before .abc otherwise we won't be able to replace it
            extraoutname = extraoutname.replace(".abc_", "")
            extraoutname = extraoutname.replace(".abc", "")
            extraoutname = extraoutname.replace("/", ".")
            extraoutname = extraoutname+"."+outname
            extraoutabc = os.path.join(output, extraoutname + ".abc")
            self.copyfile_retry(abc, extraoutabc)
            copiedExtraAbcs.append(output+"/"+extraoutname+".abc")

        for harness_abc in self.harness_abcs:
            copiedExtraAbcs.append(output+"/"+harness_abc)

        cmd = '%s -Xshell -Xoutput %s %s %s %s' % (os.path.join(self.aotsdk, 'bin/adt'), output, self.aotextraargs, " ".join(copiedExtraAbcs), outabc)
        self.verbose_print(cmd)

        try:
            (f,err,exitcode) = self.run_pipe(cmd)
            if exitcode == 0:
                # Deal with the fact that adt always returns 0, I have not actually seen it return
                # anything else even when it says that it failed. So instead of relying on the exitcode
                # actually look to see if it sucessfully linked a shell.
                if not isfile(outabc.replace(".abc", "")):
                    exitcode = 1
            for line in f:
                self.verbose_print(line.strip())
            for line in err:
                self.verbose_print(line.strip())
        except:
            raise Exception()

        # Only attempt code signing if the AOT compilation is clean up to this point
        if exitcode == 0:
            # Need to do ad-hoc code signing of the executable
            cmd = '%s -fs "-" %s' % ('codesign', ("%s/%s" % (output, outname)))
            self.verbose_print(cmd)
            (f_s,err_s,exitcode_s) = self.run_pipe(cmd)
            # If this fails raise an exception, not much we can do
            if exitcode_s != 0:
                for line in f_s:
                    print(line.strip())
                for line in err_s:
                    print(line.strip())
                raise Exception()

        return (f, err, exitcode)

    def copyfile_retry(self, src, dest, attempts=3):
        # Have had problems with AOT compilation randomly failing because not all of the
        # necessary abc files had been properly copied into the "output" directory. The
        # problem stems from isfile() returning false when it should be true. Could be related
        # to the fact that when this happens about 20 threads are starting up and all of them
        # are accessing abcasm/abs_helper.abc at the same time and randomly some of the threads
        # will get a false returned by the call to isfile().
        count = 1
        while (count <= attempts):
            if isfile(src): # Safe guard
                shutil.copyfile(src, dest)
                return
            count = count + 1
            sleep(1)
        # If we get this far, we will silently fail to copy the file. This will
        # allow the ADT call to produce its failure of not being able to open the abc file.

    # fixAbcCommand
    # cmd: a string of compiler arguments
    # return: a string or compiler arguments with all files ended with .abc_ replaced with .abc
    def fixAbcCommand(self,cmd):
        newcmd=[]
        for token in cmd.split():
            if token.endswith('.abc_') and os.path.exists(os.getcwd()+'/'+token): # if token is a .abc_ file
                tokenWithout_=token[:-1]
                if not os.path.exists(os.getcwd()+'/'+tokenWithout_):
                    shutil.copy(os.getcwd()+'/'+token,os.getcwd()+'/'+tokenWithout_)
                newcmd.append(tokenWithout_)
            else: # otherwise retain compiler parameters
                newcmd.append(token)
        return string.join(newcmd) # convert from array of string back to string

    def compile_test(self, as_file, extraArgs=[], outputCalls = None):
        self.checkExecutable(self.java, 'java must be on system path or --java must be set to java executable')
        asc, builtinabc, shellabc, ascargs = self.asc, self.builtinabc, self.shellabc, self.ascargs
        # if there is a .build file available (which is an executable script)
        # run that file instead of compiling with asc
        as_base = as_file[0:as_file.rfind('.')]
        if isfile(as_base+'.build'):
            (dir,file)=split(as_base)
            self.verbose_print('    compiling %s running %s%s' % (file, as_base, '.build'))
            (f,err,exitcode) = self.run_pipe('%s%s' % (as_base, '.build'), outputCalls=outputCalls)
            for line in f:
                self.verbose_print(line.strip())
            for line in err:
                self.verbose_print(line.strip())
            return

        if not self.ascOutputOnly and not isfile(asc):
            exit('ERROR: cannot build %s, ASC environment variable or --asc must be set to asc.jar' % as_file)

        (dir, file) = split(as_file)

        # additional .as file compiler args
        if as_file.endswith(self.sourceExt):
            if not isfile(builtinabc):
                exit('ERROR: builtin.abc (formerly global.abc) %s does not exist, BUILTINABC environment variable or --builtinabc must be set to builtin.abc' % builtinabc)

            javaArgList = parseArgStringToList(self.javaargs)
            javaArgList = self.loadArgsFile(javaArgList, dir, as_file, 'java_args')

            if asc.endswith('.jar'):
                cmd = self.java
                for arg in javaArgList:
                    cmd += ' %s' % arg
                cmd += ' -jar %s' %  asc
            else:
                cmd = asc
                
            if self.ascOutputOnly:
                # asc output is only the args passed to asc
                cmd = ''
                
            

            ascargs += ' ' + ' '.join(extraArgs)
            ascArgList = parseArgStringToList(ascargs)
            
            # look for .asc_args files to specify dir / file level compile args, arglist is passed by ref
            ascArgList = self.loadArgsFile(ascArgList, dir, as_file, 'asc_args')
            
            (testdir, ext) = splitext(as_file)
                
            cmd += ' -import %s' % builtinabc
            for arg in ascArgList:
                cmd += ' %s' % arg

            cmd=self.fixAbcCommand(cmd)            
            # look for a subdirectory of the same name as the .as file
            # if it exists, import all of the files within that dir when
            # compiling the test
            deps = glob(join(testdir,'*.abc'))
            deps.sort()
            for util in deps + glob(join(dir,'*Util'+self.sourceExt)):
                if self.osName == 'win' and not self.cygwin:
                    # don't escape $ when running windows python
                    cmd += ' -import %s' % util 
                else:
                    cmd += ' -import %s' % util.replace('$', '\$')
                
        elif as_file.endswith(self.abcasmExt):
            javaArgList = parseArgStringToList(self.javaargs)
            javaArgList = self.loadArgsFile(javaArgList, dir, as_file, 'java_args')
            
            cmd = '%s -j "%s" -a "%s"' % (self.abcasmRunner, self.java, ' '.join(javaArgList))
            
        elif as_file.endswith(self.executableExtensions):
            # directly executable file doesn't need to be compiled
            return

        if self.ascversion=='3':
            cmd=self.translateToAsc3(cmd)
        
        if self.ascOutputOnly:
            self.ascOutputFile.write('%s %s\n' % (cmd, as_file))
            return ''
        
        
        if '$' in as_file:
            if self.osName != 'win' or self.cygwin:
                as_file = as_file.replace('$', '\$')
        
        try:
            (f,err,exitcode) = self.run_pipe('%s %s' % (cmd,as_file),
                                             outputCalls=outputCalls)
            return f+err
        except:
            raise
    
    def compileWithAsh(self, tests):
        self.checkExecutable(self.java, 'java must be on system path or --java must be set to java executable')
        if not self.ascOutputOnly and not isfile(self.asc):
            exit('ERROR: cannot build, ASC environment variable or --asc must be set to asc.jar')
        if not isfile(self.builtinabc):
            exit('ERROR: builtin.abc (formerly global.abc) %s does not exist, '
                 'BUILTINABC environment variable or --builtinabc must be set '
                 'to builtin.abc' % self.builtinabc)

        start_time = datetime.today()
        #print("starting compile of %d tests at %s" % (len(tests),start_time))
        total=len(tests)
        if not pexpect_module_present or self.ascOutputOnly:
            if self.genAtsSwfs:
                print('The pexpect module must be installed to generate ats swfs.')
                exit(1)
            for test in tests:
                total -= 1
                (dir, file) = split(test)
                (testdir, ext) = splitext(test)
                settings = self.get_test_settings(testdir)
                if self.excludeTest(file, settings):
                    continue
                if self.aotsdk:
                    settings = self.get_test_settings(testdir)
                    if '.*' in settings and 'skip' in settings['.*']:
                        self.js_print('Skipping %s ... reason: %s' % (test,settings['.*']['skip']))
                        continue
                    # If the testcase is checked in as an abc then we can jump to AOT compilation
                    if test.endswith(self.abcOnlyExt):
                        #compile the abc
                        self.compile_aot( test )
                        continue
                if test.endswith(self.executableExtensions):
                    continue
                self.js_print('%d\tcompiling %s' % (total,test))
                self.compile_test(test)
                if self.ascOutputOnly:
                    continue  # no .abc created
                if not exists(testdir+".abc"):
                    print("ERROR abc files %s.abc not created" % (testdir))
                    self.ashErrors.append("abc files %s.abc not created" % (testdir))
                    continue

                if self.aotsdk: # ABC compiled, so now AOT compile
                    # Need to figure out how to deal with additional required abc files
                    self.compile_aot(testdir+".abc")

        else:  #pexpect available
            if isfile(self.ash):
                child = pexpect.spawn('"%s" %s -jar %s ' % (self.java, self.javaargs, self.ash))
            else:
                child = pexpect.spawn('"%s" %s -classpath %s macromedia.asc.embedding.Shell' % (self.java, self.javaargs, self.asc))
                
            child.logfile = None
            if self.verbose:
                child.logfile = sys.stdout
            child.expect("\(ash\)")
            child.expect("\(ash\)")

            for test in tests:
                self.js_print('compiling %s' % test)
                (dir, file) = split(test)
                (testdir, ext) = splitext(test)
                settings = self.get_test_settings(testdir)
                if self.excludeTest(file, settings):
                    continue
                if self.skipTest(file, settings):
                    continue

                if test.endswith(self.abcasmExt):
                    total -= 1
                    self.compile_test(test)
                    if exists(testdir+".abc") and self.aotsdk: # ABC compiled, so now AOT compile
                        if self.show_time:
                            start_time=time()
                        else:
                            self.js_print('AOT compiling %s' % (testdir+'.abc'))
                        (f,err,exitcode) = self.compile_aot(testdir+".abc", [self.abcasmShell+'.abc'])
                        if self.show_time:
                            self.js_print('AOT compiling %s time %.1f' % (testdir+".abc",time()-start_time))
                        if exitcode != 0:
                            self.js_print("ERROR: AOT compilation failed for %s" % (testdir+".abc"))
                            [self.js_print(line) for line in err]
                            self.ashErrors.append("AOT compilation failed for %s\n\t%s" % (testdir+".abc", '\n'.join(str(x) for x in err)))
                    continue
                elif test.endswith(self.executableExtensions):
                    total -= 1
                    if self.aotsdk: # ABC precompiled, so now AOT compile
                        # Need to figure out how to deal with additional required abc files
                        self.js_print('AOT compiling %s' % (testdir+".abc_"))
                        (f,err,exitcode) = self.compile_aot(test)
                        if exitcode != 0:
                            self.js_print("ERROR: AOT compilation failed for %s" % (testdir+".abc_"))
                            [self.js_print(line) for line in err]
                            self.ashErrors.append("AOT compilation failed for %s\n\t%s" % (testdir+".abc_", '\n'.join(str(x) for x in err)))
                    continue
                else:
                    extraAscArgs=_extraAscArgs=''
                    extraABCs=[]
                    _extraABCs=''
                    outputCalls=[]
                    if os.path.exists(testdir):
                        self.compile_support_files(testdir, outputCalls)
                        for testfile in glob(join(testdir,'*.abc')):
                            _extraABCs += ' %s' % testfile
                            _extraAscArgs += ' %s' % testfile
                        if 'Interface' in _extraAscArgs:
                            _extraAscArgs = self.sortInterfaces(_extraAscArgs)
                            _extraABCs = self.sortInterfaces(_extraABCs)
                    for abcfile in _extraAscArgs.split():
                        extraAscArgs+=' -import %s' % abcfile
                    extraABCs = _extraABCs.split();
                    
                    arglist = parseArgStringToList(self.ascargs)

                    # Check for a local .java_args file (either dir or file specific)
                    # Not ideal - but we try to load a .java_args file, and if one is loaded, then we revert to compiling
                    # without ash, as there is no way to pass new java args to the already running ash process
                    javaArgList = []
                    javaArgList = self.loadArgsFile(javaArgList, dir, test, 'java_args')
                    if javaArgList:
                        self.compile_test(test)
                    else:

                        # look for .asc_args files to specify dir / file level compile args
                        arglist = self.loadArgsFile(arglist, dir, test, 'asc_args')

                        if self.genAtsSwfs:
                            # get settings as ats excluded files are defined there
                            settings = self.get_test_settings(testdir)
                            if self.skipAtsTest(file, settings):
                                continue
                            arglist.extend(genAtsArgs(dir,file,self.atstemplate))

                        cmd = "asc -import %s " % (self.builtinabc)
                        cmd += extraAscArgs
                        for arg in arglist:
                            cmd += ' %s' % arg

                        if exists(testdir+".abc"):
                            os.unlink(testdir+".abc")

                        if self.genAtsSwfs:
                            if search_file(test+'.avm_args', 'USES_SWFVERSION'):
                                create_ats_swfversion_copy(test)
                                as_file = test + '.swfversion'
                                for swfversion in self.swfversions:
                                    new_cmd = cmd + ' -swf 200,200,version=%s %s' % (swfversion,test)
                                    new_cmd=self.fixAbcCommand(new_cmd)

                                    self.debug_print('%s %s.swfversion' % (new_cmd,test))
                                    child.sendline('%s %s.swfversion' % (new_cmd,test))
                                    child.expect("\(ash\)")
                                    # rename the file to indicate which version it was compiled to
                                    # compiled filename is test.as.swfversion.abc
                                    file_noext, ext = splitext(file)
                                    new_filename = file_noext+'_v%s' % swfversion + '.swf'
                                    shutil.move(join(dir,file)+'.swfversion.swf', join(dir, new_filename))
                                    moveAtsSwf(dir,new_filename, self.atsDir)
                                try:
                                    os.remove(join(dir,file)+'.swfversion')
                                    os.remove(join(dir,file)+'.swfversion.abc')
                                except:
                                    pass
                            else:
                                cmd += " -swf 200,200 %s" % test
                                cmd=self.fixAbcCommand(cmd)
                                self.debug_print(cmd)
                                child.sendline(cmd)
                                child.expect("\(ash\)")
                                moveAtsSwf(dir,file, self.atsDir)
                        else:
                            if self.ascversion=='3':
                                cmd=self.translateToAsc3(cmd)
                            cmd += " %s" % test
                            cmd=self.fixAbcCommand(cmd)
                            child.sendline(cmd)
                            child.expect("\(ash\)")

                            if not exists(testdir+".abc"):
                                print("ERROR: abc file %s.abc not created, cmd used to compile: %s" % (testdir,cmd))
                                self.ashErrors.append("abc file %s.abc not created, cmd used to compile: %s" % (testdir,cmd))

                    total -= 1

                    #print("%d remaining, %s" % (total,cmd))

                if self.aotsdk: # ABC compiled, so now AOT compile
                    # Need to figure out how to deal with additional required abc files
                    if self.show_time:
                        start_time=time()
                    else:
                        self.js_print('AOT compiling %s' % (testdir+".abc"))
                    (f,err,exitcode) = self.compile_aot(testdir+".abc", extraABCs)
                    if self.show_time:
                        self.js_print('AOT compiling %s %.1f' % (testdir+".abc",time()-start_time))
                    if exitcode != 0:
                        self.js_print("ERROR: AOT compilation failed for %s" % (testdir+".abc"))
                        [self.js_print(line) for line in err]
                        self.ashErrors.append("AOT compilation failed for %s\n\t%s" % (testdir+".abc", '\n'.join(str(x) for x in err)))



        end_time = datetime.today()

    def translateToAsc3(self,cmd):
        # translates the asc switches syntax from asc1,2 to asc3
        # from: java -jar asc.jar -import builtin.abc -import shell_toplevel.abc test.as
        #   to: java -jar asc.jar -builtin-external-library-path builtin.abc shell_toplevel.abc -abc -c -include-sources test.as
        self.verbose_print("translate to as3 before: %s"%cmd)
        
        tokens=cmd.split()
        newcmd=[]
        imports=['Assert.abc','Utils.abc','DateUtils.abc']
        for i in range(len(tokens)):
            if tokens[i]=="-import" and len(tokens)>i+1:
                imports.append(tokens[i+1])
            elif tokens[i] not in imports and tokens[i]!='-AS3':
                newcmd.append(tokens[i])
        if len(imports)>0:
            newcmd.append('-builtin-external-library-path')
            newcmd+=imports
        newcmd+=['-dialect','3.0','-abc','-c','-include-sources']
        cmd=string.join(newcmd)

        self.verbose_print("translate to as3 after: %s"%cmd)
        return cmd

    def compile_support_files_extra_pass(self, support_dir, abc_files, outputCalls = []):
        # One of the support files may have required another file to be imported.
        # Attempt to compile again with all the .abcs that did compile imported.
        if self.compile_support_attempts > 10:
            return
        extra_args = ['-import %s' % abcfile for abcfile in abc_files]
        compile_error = False
        for p, dirs, files in walk(support_dir):
            for f in files:
                if f.endswith(self.sourceExt):
                    f = join(p,f)
                    binFile = splitext(f)[0]+'.abc'
                    if not isfile(binFile):
                        compileOutput = self.compile_test(f, outputCalls=outputCalls, extraArgs=extra_args)
                        if not isfile(binFile) and not self.ascOutputOnly:
                            compile_error = True
                            outputCalls.append((self.js_print,('  Error compiling support file: %s' % f,)))
                            outputCalls.append((self.verbose_print, ('   compile output: %s' % compileOutput,)))
                        else:
                            if binFile not in abc_files:
                                abc_files.append(binFile)
        if compile_error:
            self.compile_support_attempts += 1
            self.compile_support_files_extra_pass(support_dir, abc_files, outputCalls=outputCalls)
    

    def compile_support_files(self, support_dir, outputCalls = []):
        compile_error = False
        abc_files = []
        self.compile_support_attempts = 1
        for p, dirs, files in walk(support_dir):
            for f in files:
                if f.endswith(self.sourceExt):
                    f = join(p,f)
                    binFile = splitext(f)[0]+'.abc'
                    if exists(binFile) and (self.forcerebuild or (self.timestampcheck and getmtime(f)>getmtime(binFile))):
                        os.unlink(binFile)
                    if not isfile(binFile):
                        compileOutput = self.compile_test(f, outputCalls=outputCalls)
                        if not isfile(binFile) and not self.ascOutputOnly:
                            compile_error = True
                        else:
                            abc_files.append(binFile)
        if compile_error:
            self.compile_support_files_extra_pass(support_dir, abc_files, outputCalls=outputCalls)

    def skipAtsTest(self, file, settings):
        '''Check testconfig if we should skip the given file.  Returns a boolean'''
        if '.*' in settings:
            for key in ['ats_skip', 'skip', 'expectedfail', 'exclude']:
                if key in settings['.*']:
                    self.js_print('ATS Skipping %s ... reason: %s' % (file,settings['.*'][key]))
                    return True
        return False

    def excludeTest(self, file, settings):
        '''Check testconfig if we should exclude the given file.  Returns a boolean'''
        if '.*' in settings:
            for key in ['exclude']:
                if key in settings['.*']:
                    self.js_print('Excluding %s ... reason: %s' % (file,settings['.*'][key]))
                    return True
        return False

    def skipTest(self, file, settings):
        '''Check testconfig if we should skip the given file.  Returns a boolean'''
        if '.*' in settings:
            for key in ['skip']:
                if key in settings['.*']:
                    self.js_print('Excluding %s ... reason: %s' % (file,settings['.*'][key]))
                    return True
        return False

    def loadArgsFile(self, arglist,dir,file, filetype='asc_args'):
        # It is possible that file is actually a partial path rooted to acceptance,
        # so make sure that we are only dealing with the actual filename
        file = split(file)[1]
        
        # file takes precendence over directory
        if file and isfile('%s/%s.%s' % (dir, file, filetype)):
            arglist = self.parseAscArgs(arglist, '%s/%s.%s' % (dir, file, filetype), dir)
        else:   # dir takes precedence over root
            # look for arg file in this or any parent dir (excluding root)
            for d in self.parents(dir):
                if d == '.':
                    break   # don't parse the root args file
                if isfile('%s/dir.%s' % (d, filetype)):
                    arglist = self.parseAscArgs(arglist, '%s/dir.%s' % (d, filetype), d)
                    break
        return arglist


    def parseAscArgs(self, currentArgList, ascArgFile, currentdir):
        # reads an .asc_args file and returns a tuple of the arg mode (override or merge) and a list of args
        f = open(ascArgFile,'r')
        while True: # skip comment lines
            ascargs = f.readline()
            if (ascargs[0] != '#'):
                break
        f.close()
        ascargs = ascargs.split('|')
        ascargs[0] = ascargs[0].strip()
        if (len(ascargs) == 1): #treat no keyword as a merge
            ascargs.insert(0,'merge')
        elif (ascargs[0] != 'override') and (ascargs[0] != 'merge'): # default to merge if mode not recognized
            ascargs[0] = 'merge'
        # replace the $DIR keyword with actual directory
        ascargs[1] = ascargs[1].replace('$DIR', currentdir)
        if ascargs[1].find('$SHELLABC') != -1:
            if not isfile(self.shellabc):   # TODO: not the best place to check for this
                exit('ERROR: shell.abc %s does not exist, SHELLABC environment variable or --shellabc must be set to shell_toplevel.abc' % self.shellabc)
            ascargs[1] = ascargs[1].replace('$SHELLABC', self.shellabc)
        ascargs[1] = parseArgStringToList(ascargs[1])
        removeArgList = self.asc_negative_args[:]   # get a copy of the negative args list
        argList = []
        for a in ascargs[1]:
            if a[0:3].lower() == '-no':
                removeArgList.append(a[3:])
            else:
                argList.append(a)

        mode = ascargs[0]
        if mode == 'merge':
            currentArgList.extend(argList)
        elif mode == 'override':
            currentArgList = argList
        # remove any duplicate args
        currentArgList = list(set(currentArgList))
        if removeArgList:
            for removeArg in removeArgList:
                try:
                    currentArgList.remove(removeArg)
                except:
                    pass

        return currentArgList


    def rebuildTests(self):
        if self.genAtsSwfs:
            if not exists(self.atsDir):
                os.mkdir(self.atsDir)

        if self.ascOutputOnly:
            self.ascOutputFile = open(self.ascOutputFilename, 'w')

        # compile any support dir files first
        if self.support_dirs:
            output_calls = []
            for sd in self.support_dirs:
                self.compile_support_files(sd, output_calls)
            self.printOutput(None, output_calls)

        self.js_print("Precompiling %s" % self.abcasmShell)
        self.compile_test(self.abcasmShell+'.as')
        # Seems odd, but it is possible for the AOT compilation to start
        # before the abcasm helper abc files is actually written out
        if self.aotsdk:
            sleep(5)
            # Make sure that the aot output directory exists before we spawn threads to compile
            if not os.path.exists(self.aotout):
                os.mkdir(self.aotout)

            # Move the harness files into the AOT compilation directory
            for harness_abc in self.harness_abcs:
                self.copyfile_retry(harness_abc, self.aotout+"/"+harness_abc)

            # We use the test config file to mark abc files that fail to AOT compile,
            # so we need to take account of that here before we try to compile them.
            self.settings, self.directives = self.parseTestConfig(self.testconfig)
            failconfig_settings, failconfig_directives = self.parseTestConfig(self.failconfig)
            self.settings.update(failconfig_settings)


        if self.threads == 1 or platform.system()[:6].upper() == 'CYGWIN':
            self.compileWithAsh(self.tests)
        else: # run using threads
            # split tests into number of threads
            testGroups = splitList(self.tests, self.threads)

            # generate threadpool
            requests = threadpool.makeRequests(self.compileWithAsh, testGroups, self.printOutput)
            main = threadpool.ThreadPool(self.threads)
            # que requests
            [main.putRequest(req) for req in requests]

            # ...and wait for the results to arrive in the result queue
            # wait() will return when results for all work requests have arrived

            try:
              main.wait()
            except (TimeOutException, KeyboardInterrupt):
                main.dismissWorkers(self.threads)
                self.killCurrentPids()
                self.lock.acquire()
                self.cleanup()
                exit(0)
            except SystemExit:
                main.dismissWorkers(self.threads)
                self.killCurrentPids()
                self.lock.acquire()
                print(sys.exc_info())
                exit(0)
        
        if self.ascOutputOnly:
            self.ascOutputFile.close()
            
        if self.genAtsSwfs:
            try:
                os.remove('./ats_temp.as')
            except:
                pass

    ### Process Management ###

    def killCurrentPids(self):
        # Kill all pids in the self.currentPids list
        self.lock.acquire()
        try:
            for p in self.currentPids:
                try:
                    p.kill()
                except:
                    pass
        finally:
            self.lock.release()

    def run_pipe(self, cmd, outputCalls=None, envVars=None):
        # run a command and return a tuple of (output, stdErr, exitCode)
        if outputCalls != None:
            outputCalls.append((self.verbose_print,(cmd,)))
        try:
            outFile = tempfile.SpooledTemporaryFile()
            errFile = tempfile.SpooledTemporaryFile()
            self.lock.acquire()
            try:
                p = Popen((cmd), shell=self.useShell, stdout=outFile, stderr=errFile, env=envVars)
                self.currentPids.append(p)
            finally:
                self.lock.release()

            starttime=time()
            output=err=[]
            while True:
                if p.poll() is None:
                    if self.testTimeOut>-1 and time()-starttime>self.testTimeOut:
                        attempts=0
                        while p.poll() is None and attempts<10:
                            p.terminate()
                            sleep(1)
                            attempts+=1
                        output=""
                        err=""
                        break
                    sleep(0.05)
                else:
                    outFile.seek(0)
                    output=outFile.read()
                    errFile.seek(0)
                    err=errFile.read()
                    break
            outFile.close()
            errFile.close()
            if output:
                output = output.decode('latin_1','replace')
                output = output.split('\n')
            else:
                output=[]
            if len(output)>0 and output[-1].strip() == '': # strip empty line at end
                output = output[:-1]

            if err:
                err = err.decode('latin_1','replace')
                err = err.split('\n')
            else:
                err=[]
            if len(err)>0 and err[-1].strip() == '':
                err = err[:-1]

            exitCode = p.returncode
            msg=''
            if exitCode == None:
                msg=' ,WARNING: could not terminate avmshell process'
                exitCode = -1

            if exitCode < 0 and self.testTimeOut>-1 and time()-starttime>self.testTimeOut:  # process timed out
                return (['process timed out after %ds%s' % (self.testTimeOut,msg)], err, exitCode)

            self.lock.acquire()
            try:
                self.currentPids.remove(p)
            finally:
                self.lock.release()

            return (output,err,exitCode)
        except KeyboardInterrupt:
            self.killCurrentPids()

    def convertBufferToString(self,output):
        output=output[2:-1]
        ptr=0
        while True:
            fnd=output.find('\\',ptr)
            if fnd==-1:
                break
            if output[fnd+1:fnd+2]=='\\':
                output=output[0:fnd]+'\\'+output[fnd+2:]
                ptr=fnd+1
            elif output[fnd+1:fnd+2]=='n':
                output=output[0:fnd]+'\n'+output[fnd+2:]
                ptr=fnd+1
            elif output[fnd+1:fnd+2]=="'":
                output=output[0:fnd]+"'"+output[fnd+2:]
                ptr=fnd+1
            elif output[fnd+1:fnd+2]=='t':
                output=output[0:fnd]+'\t'+output[fnd+2:]
                ptr=fnd+1
            elif output[fnd+1:fnd+2]=='r':
                output=output[0:fnd]+'\r'+output[fnd+2:]
                ptr=fnd+1
            elif output[fnd+1:fnd+4]=='x0c':
                output=output[0:fnd]+'\x0c'+output[fnd+4:]
                ptr=fnd+3
            elif output[fnd+1:fnd+4]=='x0b':
                output=output[0:fnd]+'\x0b'+output[fnd+4:]
                ptr=fnd+3
            else:
                ptr=fnd+1
        return output

    ### Run Tests ###

    def preProcessTests(self):  # don't need AVM if rebuilding tests
        self.js_print('current configuration: %s' % self.config, overrideQuiet=True)
        self.verbose_print(self.avm_features)
        if self.avmversion:
            self.js_print('avm version: %s' % self.avmversion)
        if self.ascversion and self.ascbuild:
            self.js_print('asc version: %s build: %s' % (self.ascversion,self.ascbuild))
        self.js_print('thread count: %d' % self.threads)
        self.js_print('Executing %d tests against vm: %s' % (len(self.tests), self.avm), overrideQuiet=True)

    def runTests(self, testList):
        testnum = len(testList) + 1
        if self.random:
            if not self.randomSeed:
                self.randomSeed = abs(hash(os.urandom(20)))     # Take the absolute val so that seeds are not negative
            self.js_print('Running tests in random order.  Random Seed = %s' % self.randomSeed)
            random.seed(self.randomSeed)
            random.shuffle(testList)

        if self.verify:
            if not re.search('debug', self.config):
                print('Avm Debugger build must be used when running --verify')
                sys.exit(1)
            if not isfile(self.abcdump+'.abc'): # check that abcdump.abc is built
                self.run_pipe('"%s" -jar %s -import %s -import %s %s' % (self.java, self.asc, self.builtinabc, self.shellabc, self.abcdump+'.as'))

        if self.timeout:
            self.js_print("will run tests until timeout of %ds is exceeded" % self.timeout)
            self.timeoutStartTime = time()

        # threads on cygwin randomly lock up
        if self.threads == 1 or platform.system()[:6].upper() == 'CYGWIN':
            for t in testList:
                testnum -= 1
                o = self.runTestPrep((t, testnum))
                self.printOutput(None, o)
                if self.timeout and time()-self.timeoutStartTime > self.timeout:
                    break
        else: # run using threads
            #Assign test numbers
            testsTuples = []
            testsLen = len(testList)
            for i,t in enumerate(testList):
              testsTuples.append([t,testsLen-i])
            # generate threadpool
            requests = threadpool.makeRequests(self.runTestPrep, testsTuples, self.printOutput, self.handle_exception)
            main = threadpool.ThreadPool(self.threads)
            # que requests
            [main.putRequest(req) for req in requests]

            try:
                while True:
                    try:
                        sleep(1)
                        main.poll()
                        if self.timeout and time()-self.timeoutStartTime > self.timeout:
                            raise TimeOutException
                        #print("(active worker threads: %i)" % (threadpool.threading.activeCount()-1, ))
                    except threadpool.NoResultsPending:
                        break
                if main.dismissedWorkers:
                    main.joinAllDismissedWorkers()
            except (TimeOutException, KeyboardInterrupt):
                main.dismissWorkers(self.threads)
                self.killCurrentPids()
                self.lock.acquire()
                self.cleanup()
                exit(0)
            except SystemExit:
                main.dismissWorkers(self.threads)
                self.killCurrentPids()
                self.lock.acquire()
                print(sys.exc_info())
                exit(0)

    #
    # cleanup
    #
    def cleanup(self):
        # Turn off quiet to display failure summary
        if self.quiet and not self.summaryonly:
            self.quiet = False

        if self.expfailmsgs:
            self.js_print('\nEXPECTED FAILURES:', '', '<br/>')
            for m in self.expfailmsgs:
                self.js_print('  %s' % m, '', '<br/>')
            self.js_print('END EXPECTED FAILURES', '', '<br/>')

        if self.unpassmsgs:
            self.js_print('\nUNEXPECTED PASSES:', '', '<br/>')
            for m in self.unpassmsgs:
                self.js_print('  %s' % m, '', '<br/>')
            self.js_print('END UNEXPECTED PASSES', '', '<br/>')

        if self.assertmsgs:
            self.js_print('\nASSERTIONS:', '', '<br/>')
            for m in self.assertmsgs:
                self.js_print('  %s' % m, '', '<br/>')
            self.js_print('END ASSERTIONS', '', '<br/>')

        if self.failmsgs:
            self.js_print('\nFAILURES:', '', '<br/>')
            for m in self.failmsgs:
                self.js_print('  %s' % m, '', '<br/>')
            self.js_print('END FAILURES', '', '<br/>')


        if self.quiet and self.summaryonly:
            self.quiet = False

        if self.rebuildtests:
            if self.ashErrors:
                self.js_print('\ntest run FAILED!')
                self.js_print('')
                self.js_print('Failed to compile %s testcases' % len(self.ashErrors))
                self.js_print('')
                self.js_print('Compile Errors:')
                for msg in self.ashErrors:
                    self.js_print('\t'+msg)
                self.js_print('')
            else:
                self.js_print('\ntest run PASSED!')
        else:
            if not self.allfails and not self.allunpass:
                self.js_print('\ntest run PASSED!')
            else:
                self.js_print('\ntest run FAILED!')

        if self.timestamps:
            end_time = datetime.today()
            self.js_print('Tests complete at %s' % end_time, '<hr><tt>', '</tt>')
            self.js_print('Start Date: %s' % self.start_time, '<tt><br>', '')
            self.js_print('End Date  : %s' % end_time, '<br>', '')
            self.js_print('Test Time : %s' % (end_time-self.start_time), '<br>', '')

        if not self.rebuildtests:
            self.js_print('passes               : %d' % self.allpasses, '<br>', '')
            self.js_print('failures             : %d' % self.allfails, '<br>', '')
            if self.allunpass>0:
                self.js_print('unexpected passes    : %d' % self.allunpass, '<br>', '')
            if self.allexpfails>0:
                self.js_print('expected failures    : %d' % self.allexpfails, '<br>', '')
            if self.allskips>0:
                self.js_print('tests skipped        : %d' % self.allskips, '<br>', '')
            if self.allexceptions>0:
                self.js_print('test exceptions      : %d' % self.allexceptions, '<br>', '')
            if self.alltimeouts>0:
                self.js_print('test timeouts        : %d' % self.alltimeouts, '<br>', '')
            if self.allasserts>0:
                self.js_print('assertions           : %d' % self.allasserts, '<br>', '')

            if self.js_output:
                print('Results were written to %s' % self.js_output)

        if self.writeResultProperties:
            logfile = open('result.properties', 'w')
            if self.allfails>0:
              logfile.write("failures=%d" % self.allfails)
            else:
              logfile.write("no failures")
            logfile.close()
        if self.junitlog!=None:
            if self.junitlogname==None:
                self.junitlogname=self.config
            convertAcceptanceToJunit.convertAcceptanceToJunit(self.js_output,self.junitlog,self.junitlogname)
            if os.path.exists(self.junitlog+'.txt'):
                os.unlink(self.junitlog+'.txt')

        if self.ashErrors and not self.cleanexit:
            exit(1)

    ### Misc Functions ###

    def getAscVersion(self, asc):
        build='unknown'
        version='unknown'
        if asc.endswith('.jar'):
            cmd = '"%s" -jar %s' % (self.java,asc)
        else:
            cmd = asc

        (f,err,exitcode) = self.run_pipe(cmd)

        try:
            build=re.compile('.*build (\d+|\S+)').search(f[1]).group(1)
        except:
            pass
        try:
            version=re.compile('[Vv]ersion (\d+)\.').search(f[1]).group(1)
        except:
            pass
        return(build,version)    

    def getAvmVersion(self, vm=None, txt=None):
        '''Pull the avm version out of the vm info or description string if provided.'''
        if vm:
            (f,err,exitcode) = self.run_pipe('%s' % vm)
            txt = f[1]
        try:
            return re.compile('.*build (\d+:\S+|\S+)').search(txt).group(1)
        except:
            return 'unknown'

    def getAvmRevision(self, avmVerStr):
        try:
            return re.compile('(\d+):').search(avmVerStr).group(1)
        except:
            return avmVerStr

    def compareAbcAsmOutput(self, file, output):
        # return diff
        try:
            f = open(file[:-4]+'.out', 'r')
            if self.config.find('debugger') != -1:
                if isfile(file[:-4]+'.out.debug'):
                    f.close()
                    f = open(file[:-4]+'.out.debug', 'r')
            if self.config.find('interp') != -1:
                if isfile(file[:-4]+'.out.interp'):
                    f.close()
                    f = open(file[:-4]+'.out.interp', 'r')
            flines = []
            for line in f.readlines():
                line = ''.join(line.split('\r'))
                if line != '\n':
                    flines.append(line)
            f.close()
        except IOError:
            flines = ['IOError Opening .out file']
        if len(output) != len(flines):
            return flines
        # compare lines
        for i in range(0,len(output)):
            line=output[i].strip()
            line=line.replace('\\','/')
            if line != flines[i].strip():
                return flines
        return

    # this will be called when an exception occurs within a thread
    def handle_exception(self, request, exc_info):
        print(traceback.print_tb(exc_info[2]))
        print(exc_info[1])
        sys.exit(1)

    def setTimestamp(self):
        if self.timestamps:
            # get the start time
            self.start_time = datetime.today()
            self.js_print('Tamarin tests started: %s' % self.start_time, overrideQuiet=True)


    def setupCEEmulators(self):
        # runs the windows mobile emulators
        # * for each threads (--numthreads=) start an emulator with emulator $EMULATOR and image $EMULATORIMAGE environment variables or use defaults below
        # * in emulator map shared directory to local machine, the shared directory gets mapped to /Storage Card on the device
        # * in shared directory /2755/autorun.exe will autorun when emulator is started
        # * copy the wmrunner.exe program as autorun.exe
        # * the wm tamarin shell is copied to /Storage Card/shell/avmshell_arm.exe
        # * the /Storage Card/nextvm.txt contains the parameters and abc file to compile
        # * /Storage Card/media is where .abc files are stored
        # * the wmrunner.exe runs in windows mobile looking for nextvm.txt, runs the avmplus_arm.exe with the arguments in the file
        # * when -log is a vm parameter the result of running the abc will be in a .log file
        # * the avm variable is replaced with the ../util/wmemulatorshell.py file, when called it copies the test abc to the wm /Storage Card/media
        #   directory and creates nextvm.txt, then waits for the wmrunner.exe to delete the nextvm.txt to signal the test is finished
        #

        emulator="c:/Program Files/Microsoft Device Emulator/1.0/DeviceEmulator.exe"
        emulator_image="c:/Program Files/Windows Mobile 6 SDK/PocketPC/DeviceemulationV650/0409/PPC_USA_GSM_VR.BIN"
        cwd=os.getcwd()
        # fixes for changing cygwin style paths into DOS
        if len(cwd)>9 and cwd[0:9]=='/cygdrive':
            cwd=cwd[9:]
        if cwd[0]=='/':
            cwd=cwd[1:]
        if cwd[1]!=':':
            cwd=cwd[0:1]+':'+cwd[1:]

        shared=cwd+"/../util/emulator_files"
        cerunner=cwd+"/../../utils/wmremote/wmrunner/Release/wmrunner.exe"
        emthreads=self.threads
        emulator_args="/memsize 128"

        if 'EMULATORTHREADS' in os.environ:
            try:
                emthreads=int(os.environ['EMULATORTHREADS'].strip())
            except:
                emthreads=self.threads
        if 'EMULATOR' in os.environ:
            emulator=os.environ['EMULATOR'].strip()
        if 'EMULATORIMAGE' in os.environ:
            emulator_image=os.environ['EMULATORIMAGE'].strip()
        if 'CERUNNER' in os.environ:
            cerunner=os.environ['CERUNNER'].strip()
        if 'EMULATORDIR' in os.environ:
            shared=os.environ['EMULATORDIR'].strip()
        if 'EMULATORARGS' in os.environ:
            emulator_args=os.environ['EMULATORARGS'].strip()

        if self.avm==None or os.path.isfile(self.avm)==False:
            print('ERROR: AVM must be set and point to the avmshell_arm.exe shell')
            sys.exit(1)
        if os.path.isfile(emulator)==False:
            print("ERROR: device emulator does not exist '%s', set EMULATOR environment variable to the correct emulator" % emulator)
            sys.exit(1)
        if os.path.isfile(emulator_image)==False:
            print("ERROR: device emulator image does not exist, '%s', set EMULATORIMAGE environment variable to the correct emulator image" % emulator_image)
            sys.exit(1)
        if os.path.isfile(cerunner)==False:
            print("ERROR: cerunner tool does not exist '%s', set CERUNNER environment variable to the correct path to cerunner.exe" % cerunner)
            sys.exit(1)
        if os.path.isdir(shared)==False:
            os.mkdir(shared)

        self.avmce=self.avm
        if 'PYTHONWIN' in os.environ:
            self.avm=os.environ['PYTHONWIN']+"  "+cwd+"/../util/wmemulatorshell.py"
        else:
            self.avm=cwd+"/../util/wmemulatorshell.py"
        # create and build version.as
        versionabc=cwd+"/version.abc"
        versionfile=open(cwd+"/version.as",'w')
        versionfile.write('import avmplus.*; print(System.getAvmplusVersion());\n')
        versionfile.close()
        self.compile_test(cwd+'/version.as')
        if os.path.isfile(versionabc)==False:
            print("ERROR: compiling %s/version.as" % cwd)
            sys.exit(1)
        if os.path.isdir(shared)==False:
            os.mkdir(shared)
        if len(os.listdir(shared))>self.threads:
            print("ERROR: emulator directory '%s' must be removed before running tests" % shared)
            sys.exit(1)

        for num in range(emthreads):
            sharedir="%s/share%d" %(shared,num)
            if os.path.isdir(sharedir)==False:
                os.mkdir(sharedir)
            if os.path.isfile(sharedir+'/lock'):
                print("WARNING: the emulator has an open lock file, removing it")
                os.unlink(sharedir+"/lock")
            if os.path.isfile(sharedir+'/running.txt'):
                print("detected emulator is already running, if not delete %s/running.txt" % sharedir)
            if os.path.isdir(sharedir+"/shell")==False:
                os.mkdir(sharedir+"/shell")
            if os.path.isdir(sharedir+"/media")==False:
                os.mkdir(sharedir+"/media")
            # 2577 is the windows mobile autorun directory
            if os.path.isdir(sharedir+"/2577")==False:
                os.mkdir(sharedir+"/2577")
                shutil.copy(cerunner,sharedir+"/2577/autorun.exe")
                shutil.copy(versionabc,sharedir+"/media/version.abc")
            shutil.copy(self.avmce,sharedir+"/shell/avmshell_arm.exe")
            file=open(sharedir+"/nextvm.txt","w")
            file.write(" -log \"\\Storage Card\\media\\version.abc\" ")
            file.close()

        winceProcesses=range(emthreads)
        for num in range(emthreads):
            if os.path.isfile("%s/share%d/running.txt" % (shared,num)):
                winceProcesses[num]="manual"
            else:
                args=[emulator]+emulator_args.split()+['/sharedfolder','%s/share%d' %(shared,num),emulator_image]
                winceProcesses[num]=subprocess.Popen(args)

        versions=range(emthreads)
        timestart=time()
        while len(versions)>0:
            for i in range(emthreads):
                versionfile="%s/share%d/media/version.log" % (shared,i)
                nextfile="%s/share%d/nextvm.txt" % (shared,i)
                if i in versions and os.path.isfile(nextfile)==False:
                    versions.remove(i)
                    try:
                        f=open(versionfile,"r")
                        data=f.read().strip()
                        f.close()
                    except:
                        data='unknown'
                    print(" emulator %d shell version => %s" % (i,data))
            if time()-timestart>60*5:
                print("ERROR: emulators %s did not start after 5 minutes" % versions)
                print("I'm guessing deleting the files '%s/share?/running.txt' will fix the problem" % (shared))
                sys.exit(1)
            sleep(.1)
        # cleanup version.as and version.abc
        try:
            if exists(cwd+"/version.as"):
                os.unlink(cwd+"/version.as")
            if exists(cwd+"/version.abc"):
                os.unlink(cwd+"/version.abc")
        except:
            print('exception deleting %s/version.as or %s/version.abc' % (cwd,cwd))
