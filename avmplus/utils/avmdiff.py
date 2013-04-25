#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=4 sw=4 expandtab:

# python modules
import random, os, time, signal, getopt, sys, subprocess, difflib, re, platform
from getopt import getopt
from sys import argv, exit, stderr
from os.path import basename, exists, join, dirname
from subprocess import Popen, PIPE, STDOUT
from difflib import unified_diff

topdir = ""

quiet = False
verbose = False
(avmr,avmrd,avmd,avmdd)=('','','','')

PY3 = sys.version_info >= (3,0)

# read the list of vm's to use from filename
# todo: support wildcards
#
# buildfile syntax:
# [builds]
#   <list of exe paths>
#
# [options]
#   list of options, one group per line

def read_buildfile(filename):
    f = open(filename, 'r')
    try:
        builds = []
        options = []
        combined = []
        list = None
        for l in f.readlines():
            l = l.strip()
            if len(l) == 0 or l[0] == '#':
                continue
            elif l == '[builds]':
                list = builds
            elif l == '[options]':
                list = options
            elif l == '[combined]':
                list = combined
            else:
                if l == '-':
                    l = ''
                l=re.sub('{shell_release}',avmr,l)
                l=re.sub('{shell_release_debugger}',avmrd,l)
                l=re.sub('{shell_debug}',avmd,l)
                l=re.sub('{shell_debug_debugger}',avmdd,l)
                list.append(l)
        return builds, options, combined
    finally:
        f.close()

# make_vmlist: combine all the option choices with all the build choices
#    and return a list of len(builds) * len(options) command lines.
def make_vmlist(buildfile):
    builds, options, combined = read_buildfile(buildfile)
    missing = [p for p in builds if not exists(p)]
    if len(missing) > 0:
        for p in missing:
            sys.stderr.write("no such file: %s\n" % p)
        exit(1)
    vms = []
    for build in builds:
        for o in options:
            vms.append('%s %s' % (build, o))
    vms = vms + combined
    verbose_print('created %d configurations' % len(vms))
    if verbose:
        for v in vms:
            print(v)
    
    return vms
try:
    signames = {
       # if you see a process die with SIG123, then add more entries below
       signal.SIGINT:  'SIGINT',
       signal.SIGILL:  'SIGILL',
       signal.SIGTERM: 'SIGTERM',
       signal.SIGABRT: 'SIGABRT',
       signal.SIGKILL: 'SIGKILL',
       signal.SIGBUS:  'SIGBUS',
    }
except:
    signames = {
       signal.SIGINT:  'SIGINT',
       signal.SIGILL:  'SIGILL',
       signal.SIGTERM: 'SIGTERM',
       signal.SIGABRT: 'SIGABRT',
    }

def signame(sig):
    if sig in signames:
        return signames[sig]
    else:
        return "SIG%d" % sig

def abcdump(filename):
    os.system('~/hg/tamarin-redux/objdir-release/shell/avmshell ~/hg/tamarin-redux/utils/abcdump.abc -- ' + filename)

def avm(vm, avmshell_args, test_args):
    cmd = '%s %s %s' % (vm, avmshell_args, test_args)
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)
    output = ()
    try:
        for line in p.stdout:
            if PY3:
                # need to decode byte-string to string
                line = line.decode('latin-1', 'replace')
            line=scrub_errors(line.strip())
            if len(line)>0:
                output = output + (line,)
    finally:
        p.stdout.close()
        stat = p.wait()
        if stat >= 128:
            stat = -(stat & ~128)
        return (stat, output)

def describe(e):
    stat, out = e
    s = ""
    #for l in out:
        #s += l
    if stat < 0:
        sig = -stat
        if s != "":
            s += '\n'
        s += "%s" % signame(sig)
    elif stat != 0:
        if s != "":
            s += '\n'
        s += "EXIT %d" % stat
    return s

def scrub_passfail(line):
# removed PASSED and FAILED messages interfering with runtests counting PASSES and FAILS tests
    line=re.sub('PASSED','P***ED',line)
    line=re.sub('FAILED','F***ED',line)
    line=re.sub('Assertion failed','A**ertion failed',line)
    return line
    
def scrub_errors(line):
# remove Error information only appearing in debugger builds
    match=re.search('[\w]+Error: Error #[0-9]+:',line)
    if match:
        end=match.end()-1
        newline=line[0:end]
        match2=line.find(') = Passed',end)
        if match2>-1:
            newline+=line[match2:]
        elif re.search('PASSED!',line):
            newline+=' PASSED!'
            newline+='\n'
        line=newline
# remove stack traces only appearing in debugger builds
    if re.search('at [\w]+()',line):
        line=''
# remove lines beginning with metric, variable data like timing should use this prefix
    if re.search('^metric ',line):
        line=''
    return line

def verbose_print(s):
    if verbose:
        print(s)

def compare(vmlist, avmshell_args, test_args):
    results = {}
    if len(vmlist) < 2:
        sys.stderr.write("too few vms were given")
        exit(1)
    for vm in vmlist:
        verbose_print('%s %s %s' % (vm, avmshell_args, test_args))
        e = avm(vm, avmshell_args, test_args)
        verbose_print(e)
        if e in results:
            results[e] += [vm]
        else:
            results[e] = [vm]
    return results

def printlines(seq):
    for l in seq:
        try:
            print('%s ' % scrub_passfail(l))
        except UnicodeEncodeError:
            # cygwin can not handle printing certain invalid unicode chars
            pass

def pick_majoirty(results):
    # pick the result with the most examples
    e0 = ()
    maxlen = 0
    for e in results:
        if len(results[e]) > maxlen:
            e0 = (e,)
            maxlen = len(results[e])
        elif len(results[e]) == maxlen:
            e0 += (e,)
    if len(e0) == 1:
        return e0[0]
    # need a tiebreaker, prefer stat==0 over nonzero
    l = [(stat,out) for stat,out in e0 if stat == 0]
    if len(l) > 0:
        return l[0]
    # prefer stat > 0 (error) over stat < 0 (crash)
    l = [(stat,out) for stat,out in e0 if stat > 0]
    if len(l) > 0:
        return l[0]
    # just pick the first one
    return l[0]

def test(vmlist, avmshell_args, test_args):
    results = compare(vmlist, avmshell_args, test_args)
    e0 = pick_majoirty(results)
    stat0, out0 = e0
    if len(results) != 1:
        # results not all the same.  print the outliers as diffs against the majority
        print(avmshell_args, test_args, 'FAILED! diff')
        for e in results:
            if e != e0:
                out0 = out0 + (describe(e0)+'\n',)
                out = e[1] + (describe(e)+'\n',)
                printlines(unified_diff(out0, out, str(results[e0]), str(results[e])))
        return 1
    elif stat0 < 0:
        # everyone crashed the same way
        print(avmshell_args, test_args, 'FAILED!', describe(e0))
        printlines(out0)
        return stat0
    if quiet:
        return 0
    # all agree, print output and return exit code
    printlines(out0)
    return stat0

def usage(stat):
    print('usage: %s [hqf]' % basename(argv[0]))
    print(' -h --help        print this message')
    print('    --buildfile=  specify file with list of vms to compare, default is "avmdiff.cfg"')
    print('                  MUST be defined using an equal (=) sign after --buildfile')
    print(' -q --quiet       if vms agree, print "PASSED!" and return 0 instead of the vm output')
    print(' -v --verbose     print extra info for diagnosing problems')
    exit(stat)

if __name__ == '__main__':
    cwd = os.path.dirname(sys.argv[0])
    buildfile = cwd+'/avmdiff.cfg'
    
    # pull out any extra options passed in.  Those go to the avmshell.
    sys_args = argv[1:]
    avmshell_args = []
    avmdiff_args = []
    test_args = []  # args to be passed to the testcase using --
    
    short_args = 'hqv'
    long_args = ['help', 'buildfile=', 'quiet', 'verbose']
    
    # The following for loop separates out avmdiff, avmshell and test args.
    # Note that the way the code is setup places limitations on avmdiff args:
    #   1. short_args arguments can NOT pass in any arguments
    #   2. long_args arguments MUST be followed by an = sign
    #   3. avmdiff args can NOT be named the same as any avmshell argument
    
    for arg in sys_args:
        if arg is '--' or test_args:
            # -- indicates the start of test_args, then once the test_args list
            # is populated it will evaluate to True and all following args will
            # be passed in
            test_args.append(arg)
        elif arg.startswith('--'):
            # extract only the arg name: remove '--' and '=.*' if present
            argname = arg[2:arg.find('=')] if '=' in arg else arg[2:]
            # compare against long list with '=' removed
            if argname in [a.strip('=') for a in long_args]:
                avmdiff_args.append(arg)
            else:
                avmshell_args.append(arg)
        elif arg.startswith('-'):
            if arg[1] in short_args:
                avmdiff_args.append(arg)
            else:
                avmshell_args.append(arg)
        else:
            avmshell_args.append(arg)
    
    avmshell_args = ' '.join(avmshell_args)
    test_args = ' '.join(test_args)
        
    try:
        # any extra args have already been pushed into avmshell_args above
        # so throw_away will never be populated
        opts, throw_away = getopt(avmdiff_args, short_args, long_args)
    except:
        usage(2)

    for o, v in opts:
        if o in ('-h', '--help'):
            usage(0)
        elif o in ('-q', '--quiet'):
            quiet = True
        elif o in ('-f', '--buildfile'):
            buildfile = v
        elif o in ('-v', '--verbose'):
            verbose = True

    if os.path.isfile(buildfile)==False:
        print("ERROR: Buildfile does not exist: '%s'" % buildfile)
        exit(1)

    exe=''
    if re.search('(CYGWIN_NT|Windows)',platform.system()):
        exe='.exe'
    avmr=cwd+'/../objdir-release/shell/avmshell'+exe
    avmrd=cwd+'/../objdir-releasedebugger/shell/avmshell'+exe
    avmd=cwd+'/../objdir-debug/shell/avmshell'+exe
    avmdd=cwd+'/../objdir-debugdebugger/shell/avmshell'+exe
    if 'shell_release' in os.environ:
        avmr=os.environ['shell_release']
    if 'shell_release_debugger' in os.environ:
        avmrd=os.environ['shell_release_debugger']
    if 'shell_debug' in os.environ:
        avmd=os.environ['shell_debug']
    if 'shell_debug_debugger' in os.environ:
        avmdd=os.environ['shell_debug_debugger']

    vmlist = make_vmlist(buildfile)
    stat = test(vmlist, avmshell_args, test_args)
    if stat == 0:
        print('PASSED! all configs match')
    exit(stat)
