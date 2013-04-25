# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

import re
import pprint
import platform
import os
from os.path import join, isdir, islink, splitext, exists
from math import sqrt
import subprocess
import shutil
import sys
import textwrap

class TimeOutException(Exception):
    def __init__(self):
        pass

# functions that all can be used independently

def convertToCsv(line):
        # convert all whitespace and [ : ] chars to ,
        line = line.replace('\n', '')
        line = line.replace('[',' ')
        line = line.replace(']',' ')
        line = line.replace(':',' ')
        lineList = []
        
        for l in line.split():
            l = l.strip()
            if l.find(',') != -1:   #wrap with "" if there is a , in val
                l = '"%s"' % l
            lineList.append(l)
        
        return ','.join(lineList)

def detectCPUs():
    """
    Detects the number of CPUs on a system.
    """
    # Linux, Unix and MacOS:
    if hasattr(os, "sysconf"):
        if "SC_NPROCESSORS_ONLN" in os.sysconf_names:
            # Linux & Unix:
            ncpus = os.sysconf("SC_NPROCESSORS_ONLN")
            if isinstance(ncpus, int) and ncpus > 0:
                return ncpus
        else: # OSX:
            p = subprocess.Popen("sysctl -n hw.ncpu", shell=True, close_fds=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            return int(p.stdout.read())
    # Windows:
    if "NUMBER_OF_PROCESSORS" in os.environ:
        ncpus = int(os.environ["NUMBER_OF_PROCESSORS"]);
        if ncpus > 0:
            return ncpus
    return 1 # Default  

def dict_match(dict,test,value):
    for k in dict.keys():
        if re.search(k,test):
            if value in dict[k]:
                return dict[k][value]

def formatMemoryList(lst):
    out=""
    for mem in lst:
        out="%s%s, " %(out,formatMemory(mem))
    out=out[:-2]
    return out

def formatMemory(mem):
    if mem<1024:
        out="%dK" % mem
    else:
        out="%.1fM" % (mem/1024.0)
    return  out

def list_match(list,test):
    for k in list:
        if re.search(k,test):
            return True
    return False

def parseArgStringToList(argStr):
    # Convert an argument string to a list
    args = argStr.strip().split(' ')
    # recombine any args that have spaces in them
    argList = []
    for a in args:
        if a == '':
            pass
        elif a[0] == '-':
            argList.append(a)
        else:   # append the space and text to the last arg
            argList[len(argList)-1] += ' ' + a
    return argList

def pPrint(obj):
    pprint.pprint(obj)
 
def pythonVersion26():
    '''Return boolean indicating if python version >= 2.6'''
    return sys.version_info[0] == 2 and sys.version_info[1] >= 6
                    
    
def search_file(filename, search_text):
    '''Return a boolean indicating whether or not text is present in file.
        (If file exists).'''
    if not os.path.exists(filename):
        return False
    f = open(filename, 'r')
    for line in f:
        if search_text in line:
            f.close()
            return True
    f.close()
    return False
    

def splitList(_list, _groups):
    ''' Split the list of tests to compile using round-robin instead of previous method
    which would just break the number of tests into '_groups' number of large chunks. The
    thought here is that it is more likely that tests that take a long time to compile or
    are being excluded are probably grouped together. Previously it was possible to have
    an entire thread assigned the as3/Types/Float* tests, which are currently excluded
    (as of 02/14/2012), which means that you have an entire thread that is not going to
    compile anything.
    '''
    groups=[]
    for x in range(_groups):
        groups.append([])
    i=0
    for test in _list:
        groups[i].append(test)
        i = 0 if i >= (_groups-1) else (i+1)
    return groups

### ATS Support Functions ###

def create_ats_swfversion_copy(file):
    '''Create a copy of the given .as file to use the ATS compatible swfVersion
        instead of avmplus.System.swfversion.  Also wrap the test with code
        to not run the test until fully loaded.
        (Tamarin tests run in the unnamed package and therefore the code runs
        before the tests INIT handler has fired.  However, the swfVersion of the
        test is not available until after the INIT event.)
        '''
    header = '''
        import flash.events.*;
        var clInfo = ATS.client.swfLoader.contentLoaderInfo;
        clInfo.addEventListener(Event.INIT, swfVersionInitHandler);
        
        function swfVersionInitHandler(event):void {
            swfVersion = clInfo.swfVersion
            run_swfversion_test();
        }
        
        function run_swfversion_test():void {
        '''
    footer = '\n}\n'
    
    orig_file = open(file, 'r')
    swfversion_file = open(file+'.swfversion', 'w')
    
    swfversion_file.write(textwrap.dedent(header))
    for line in orig_file.readlines():
        swfversion_file.write(line.replace('System.swfVersion', 'swfVersion')+'\n')
    swfversion_file.write(footer)
    
    orig_file.close()
    swfversion_file.close()
    
    

def genAtsArgs(dir, file, atstemplate):
    args = []
    (name, ext) = splitext(file)
    # insert filename into template
    atstemplate[2] = 'var fileName:String = "%s_";\n' % name
    # write out the template file
    ats_inc = open('./ats_temp.as','w')
    ats_inc.writelines(atstemplate)
    ats_inc.close()
    if not 'ecma' in dir:
        args.append('-AS3')
    args.append('-in ./ats_temp.as')
    return args
    
def moveAtsSwf(dir, file, atsDir):
    # move the swf to the swfs dir
    (name, ext) = splitext(file)
    try:
        atsOut = atsDir+'/'+dir 
        if not exists(atsOut):
            os.makedirs(atsOut)
    except:
        pass
        
    try:
        shutil.move('%s/%s.swf' % (dir,name),'%s/%s_.swf' % (atsOut,name))
    except IOError:
        print('Error attempting to move %s/%s_.swf' % (dir,name))
    except:
        raise


### Statistical Functions ###

def conf95(list):
    if mean(list) == 0:
        return 0
    return ((tDist(len(list)) * standard_error(list) / mean(list)) * 100)
    
def mean(population):
    mean = 0.0
    for x in population:
        mean = mean + float(x)
    return mean / len(population)

def median(pool):
    copy = sorted(pool)
    size = len(copy)
    if size % 2 == 1:
        return copy[(size - 1) / 2]
    else:
        return (copy[size/2 - 1] + copy[size/2]) / 2
    

def rel_std_dev(population):
    # Relative Standard Deviation = % standard deviation
    try:
        return (100*standard_deviation(population))/mean(population)
    except ZeroDivisionError:
        return 0.0

def standard_deviation(population):
    return sqrt(variance(population))

def standard_error(population):
    return standard_deviation(population) / sqrt(len(population))
    
def tDist(n):
    tDistribution = [ 999, 999, 12.71, 4.30, 3.18, 2.78, 2.57, 2.45, 2.36, 2.31, 2.26, 2.23, 2.20, 2.18, 2.16, 2.14, 2.13, 2.12, 2.11, 2.10, 2.09, 2.09, 2.08, 2.07, 2.07, 2.06, 2.06, 2.06, 2.05, 2.05, 2.05, 2.04, 2.04, 2.04, 2.03, 2.03, 2.03, 2.03, 2.03, 2.02, 2.02, 2.02, 2.02, 2.02, 2.02, 2.02, 2.01, 2.01, 2.01, 2.01, 2.01, 2.01, 2.01, 2.01, 2.01, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 2.00, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.99, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.98, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.97, 1.96]
    tMax = len(tDistribution)
    tLimit = 1.96
    if (n > tMax):
        return tLimit
    return tDistribution[n]

def variance(population):
    n = 0
    mean = 0.0
    s = 0.0
    for x in population:
        n = n + 1
        delta = int(x) - mean
        mean = mean + (delta / n)
        s = s + delta * (int(x) - mean)

    # if you want to calculate std deviation
    # of a sample change this to "s / (n-1)"
    return s / (n)


### os.path replacement functions ###

def join(a,b):
    return a+'/'+b

def walk(top, topdown=True, onerror=None, followlinks=False):
    '''Updated version of os.path.walk that will follow symbolic links'''

    try:
        names = os.listdir(top)
        names.sort()
    except os.error:
        if onerror is not None:
            onerror(sys.excinfo()[0])
        return

    dirs, nondirs = [], []
    for name in names:
        if isdir(join(top, name)):
            dirs.append(name)
        else:
            nondirs.append(name)

    if topdown:
        yield top, dirs, nondirs
    for name in dirs:
        path = join(top, name)
        if followlinks or not islink(path):
            for x in walk(path, topdown, onerror, followlinks):
                yield x
    if not topdown:
        yield top, dirs, nondirs
              




### Dictionary of signal (exit code) names ###
# list scraped from "kill -l"
signalNames = {
    1 : 'SIGHUP',
    2 : 'SIGINT',
    3 : 'SIGQUIT',
    4 : 'SIGILL',
    5 : 'SIGTRAP',
    6 : 'SIGABRT',
    7 : 'SIGEMT',
    8 : 'SIGFPE',
    9 : 'SIGKILL',
    10 : 'SIGBUS',
    11 : 'SIGSEGV',
    12 : 'SIGSYS',
    13 : 'SIGPIPE',
    14 : 'SIGALRM',
    15 : 'SIGTERM',
    16 : 'SIGURG',
    17 : 'SIGSTOP',
    18 : 'SIGTSTP',
    19 : 'SIGCONT',
    20 : 'SIGCHLD',
    21 : 'SIGTTIN',
    22 : 'SIGTTOU',
    23 : 'SIGIO',
    24 : 'SIGXCPU',
    25 : 'SIGXFSZ',
    26 : 'SIGVTALRM',
    27 : 'SIGPROF',
    28 : 'SIGWINCH',
    29 : 'SIGINFO',
    30 : 'SIGUSR1',
    31 : 'SIGUSR2'
}

def getSignalName(code):
    if code in signalNames:
        return signalNames[code]
    else:
        return "Not Found"
